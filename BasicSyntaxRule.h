#pragma once
//BasicSyntaxRule:20130617
#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <utility>
#include <cassert>
#include "TokenBuffer.h"

namespace parsia
{
template<class ReturnType>
class BasicSyntaxRule : 
		public std::enable_shared_from_this<BasicSyntaxRule<ReturnType>> {
public:
	using Ptr = std::shared_ptr<BasicSyntaxRule>;
	using RuleProcessor = 
		std::function<const ReturnType (const std::string& rule_name)>;
	using Choice = std::function<
		const ReturnType (const TokenBuffer::Ptr&, const RuleProcessor&)>;

	static auto Create() -> Ptr {
		return Ptr(new BasicSyntaxRule());	
	}

	auto AddChoice(const Choice& choice) -> Ptr {
		choice_list_.push_back(choice);
		return BasicSyntaxRule<ReturnType>::shared_from_this();
	}

	auto ProcessRule(const TokenBuffer::Ptr& token_buffer, 
			const RuleProcessor& rule_processor) -> ReturnType {
		if(!token_buffer->IsSpeculating()){
			memo_.clear();
		}
		return ProcessRuleWithMemoization(token_buffer, rule_processor);
	}

private:
	class MemoInformation{
	public:
		MemoInformation() : is_success_(false), end_token_index_(0){} 
		MemoInformation(bool is_success, int end_token_index) : 
			is_success_(is_success), end_token_index_(end_token_index){}

		auto GetIsSuccess()const -> bool {
			return is_success_;	
		}

		auto GetEndTokenIndex()const -> int {
			return end_token_index_;	
		}

	private:
		bool is_success_;
		int end_token_index_;
	};

	BasicSyntaxRule() : choice_list_(), memo_(){}

	auto SpeculatingChoice(const Choice& choice, 
			const TokenBuffer::Ptr& token_buffer, 
			const RuleProcessor& rule_processor)const -> bool {
		bool is_success = true;
		token_buffer->MarkIndex();
		try {
			choice(token_buffer, rule_processor);	
		}
		catch(const SyntaxError& e){
			token_buffer->DebugPrint("ChoiceError");
			is_success = false;
		}
		token_buffer->ReturnToLastMarkedIndex();
		return is_success;
	}

	auto DoProcessRule(const TokenBuffer::Ptr& token_buffer, 
			const RuleProcessor& rule_processor) -> ReturnType {
		if(choice_list_.size()==1){
			return choice_list_.front()(token_buffer, rule_processor);
		}
		else { //Start speculating
			for(auto choice : choice_list_){
				if(SpeculatingChoice(choice, token_buffer, rule_processor)){
					return choice(token_buffer, rule_processor);	
				}
			}
			throw SyntaxError("SyntaxError: invalid syntax.");
		}
		assert(!"never read this line.");
		return ReturnType();
	}

	
	auto Memoize(const TokenBuffer::Ptr& token_buffer, 
			int start_token_index, bool is_success) -> void {
		memo_.insert(std::pair<int, MemoInformation>(start_token_index, 
			MemoInformation(is_success, token_buffer->GetLookAheadIndex())));
	}

	auto ProcessRuleWithMemoization(const TokenBuffer::Ptr& token_buffer, 
			const RuleProcessor& rule_processor) -> ReturnType {
		const auto start_token_index = token_buffer->GetLookAheadIndex();
		if(token_buffer->IsSpeculating()){
			const auto memo_info_iter = memo_.find(start_token_index);
			if(memo_info_iter != memo_.end()){
				if(memo_info_iter->second.GetIsSuccess()){
					token_buffer->SetLookAheadIndex(
						memo_info_iter->second.GetEndTokenIndex());
					token_buffer->DebugPrint("short cut by memo.");
					return ReturnType();
				}
				else {
					throw SyntaxError("SyntaxError: checked this error before.");	
				}
			}
		}
		auto when_speculating_memoizer = 
			[this, &token_buffer, start_token_index](bool is_success) -> void {
				if(token_buffer->IsSpeculating()){
					Memoize(token_buffer, start_token_index, is_success);
				}	
			};
		try{
			auto tree = DoProcessRule(token_buffer, rule_processor);
			when_speculating_memoizer(true);
			return tree;
		}catch(const SyntaxError& e){
			when_speculating_memoizer(false);
			throw;
		}
	}
	
	
	std::vector<Choice> choice_list_;
	std::map<int, const MemoInformation> memo_;

};
}

