#pragma once
//BasicSyntaxRule:20130617
#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <utility>
#include <cassert>
#include "BasicTokenBuffer.h"

namespace parsia
{
template<class Token, class TokenType, class ReturnType>
class BasicSyntaxRule : 
	public std::enable_shared_from_this<BasicSyntaxRule<Token, TokenType, ReturnType>> {
public:
	using Ptr = std::shared_ptr<BasicSyntaxRule>;
	using TokenMatcher = std::function<const ReturnType (const TokenType)>;
	using AheadTokenLooker = std::function<const Token (unsigned int)>;
	using RuleProcessor = 
		std::function<const ReturnType (const std::string& rule_name)>;
	using Choice = std::function<const ReturnType (
		const TokenMatcher&, const AheadTokenLooker&, const RuleProcessor&)>;

	static auto Create(const BasicTokenBuffer<Token, TokenType>& token_buffer) -> Ptr {
		return Ptr(new BasicSyntaxRule(token_buffer));	
	}

	auto AddChoice(const Choice& choice) -> const Ptr {
		choice_list_.push_back(choice);
		return BasicSyntaxRule<Token, TokenType, ReturnType>::shared_from_this();
	}

	auto ProcessRule(const RuleProcessor& rule_processor) -> const ReturnType {
		if(!token_buffer_->IsSpeculating()){
			memo_.clear();
		}
		return ProcessRuleWithMemoization(rule_processor);
	}

private:
	class MemoInformation{
	public:
		MemoInformation() : is_success_(false), end_token_index_(0){} 
		MemoInformation(bool is_success, int end_token_index) : 
			is_success_(is_success), end_token_index_(end_token_index){}
		auto GetIsSuccess()const -> const bool { return is_success_; }
		auto GetEndTokenIndex()const -> const int { return end_token_index_; }
	private:
		bool is_success_;
		int end_token_index_;
	};

	BasicSyntaxRule(const typename BasicTokenBuffer<Token, TokenType>::Ptr& token_buffer) :
		token_buffer_(token_buffer),
		token_matcher_([this](const Token& token, const TokenType& type) -> const Token {
			return token_buffer_->Match(token, type);
		}),
		ahead_token_looker_([this](unsigned int index) -> const Token {
			return token_buffer_->LookAheadToken(index);
		}),
		choice_list_(), memo_(){}

	auto SpeculatingChoice(const RuleProcessor& rule_processor, 
			const Choice& choice)const -> bool {
		bool is_success = true;
		token_buffer_->MarkIndex();
		try {
			choice(token_matcher_, ahead_token_looker_, rule_processor);	
		}
		catch(const SyntaxError& e){
			token_buffer_->DebugPrint("ChoiceError");
			is_success = false;
		}
		token_buffer_->ReturnToLastMarkedIndex();
		return is_success;
	}

	auto DoProcessRule(const RuleProcessor& rule_processor) -> ReturnType {
		if(choice_list_.size()==1){
			return choice_list_.front()(
				token_matcher_, ahead_token_looker_, rule_processor);
		}
		else { //Start speculating
			for(const auto& choice : choice_list_){
				if(SpeculatingChoice()){
					return choice(token_matcher_, ahead_token_looker_, rule_processor);	
				}
			}
			throw SyntaxError("SyntaxError: invalid syntax.");
		}
		assert(!"never read this line.");
		return ReturnType();
	}

	
	auto Memoize(int start_token_index, bool is_success) -> void {
		memo_.insert(std::pair<int, MemoInformation>(start_token_index, 
			MemoInformation(is_success, token_buffer_->GetLookAheadTokenIndex())));
	}

	auto ProcessRuleWithMemoization(const RuleProcessor& rule_processor) -> ReturnType {
		const auto start_token_index = token_buffer_->GetLookAheadIndex();
		if(token_buffer_->IsSpeculating()){
			const auto memo_info_iter = memo_.find(start_token_index);
			if(memo_info_iter != memo_.end()){
				if(memo_info_iter->second.GetIsSuccess()){
					token_buffer_->SetLookAheadIndex(
						memo_info_iter->second.GetEndTokenIndex());
					token_buffer_->DebugPrint("short cut by memo.");
					return ReturnType();
				}
				else {
					throw SyntaxError("SyntaxError: checked this error before.");	
				}
			}
		}
		auto when_speculating_memoizer = 
			[this, start_token_index](bool is_success) -> void {
				if(token_buffer_->IsSpeculating()){
					Memoize(token_buffer_, start_token_index, is_success);
				}	
			};
		try{
			auto tree = DoProcessRule(rule_processor);
			when_speculating_memoizer(true);
			return tree;
		}catch(const SyntaxError& e){
			when_speculating_memoizer(false);
			throw;
		}
	}
	

	typename BasicTokenBuffer<Token, TokenType>::Ptr token_buffer_;
	TokenMatcher token_matcher_;
	AheadTokenLooker ahead_token_looker_;
	std::vector<Choice> choice_list_;
	std::map<int, const MemoInformation> memo_;
};
}

