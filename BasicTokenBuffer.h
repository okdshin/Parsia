#pragma once
//BasicTokenBuffer:20130611
#include <iostream>
#include <memory>
#include <functional>
#include <vector>
#include <cassert>
#include <sstream>

namespace parsia
{

class SyntaxError : public std::exception {
public:
	SyntaxError(const std::string& message) : 
		message_(message) {}
	virtual ~SyntaxError() throw() {}
	virtual const char* what() const throw() {
		return message_.c_str();
	}
private:
	const std::string message_;
};

template<class Token, class TokenType>
class BasicTokenBuffer{
public:
	using Ptr = std::shared_ptr<BasicTokenBuffer>;
	using NextTokenGetter = std::function<Token ()>;
	using IsTokenTypeSameDecider = std::function<bool (const Token&, const TokenType&)>;
	using TokenOutputter = 
		std::function<void (std::ostream&, const Token& token)>;
	using TokenTypeOutputter = 
		std::function<void (std::ostream&, const TokenType& token)>;

	static auto Create(const NextTokenGetter& next_token_getter,
			const IsTokenTypeSameDecider& is_token_type_same_decider,
			const TokenOutputter& token_outputter,
			const TokenTypeOutputter& token_type_outputter) -> Ptr {
		return Ptr(new BasicTokenBuffer(
			next_token_getter, is_token_type_same_decider, 
			token_outputter, token_type_outputter));	
	}

	auto DebugPrint(const std::string& text) -> void {
#ifdef PARSIA_TOKEN_BUFFER_DEBUG_ON
		if(IsSpeculating()){
			std::cout << "\033[36mSpeculating: \033[39m" << text << std::endl;
		}
		else {
			std::cout << text << std::endl;	
		}
		std::cout << "\t";
		for(unsigned int i = 0; i < look_ahead_token_list_.size(); ++i){
			if(i == look_ahead_index_){
				std::cout << "\033[31m@\033[39m"; 
			}
			token_outputter_(std::cout, look_ahead_token_list_[i]);
			std::cout << " ";
		}
		
		std::cout << "\n" << std::endl;
#endif
	}

	auto GetLookAheadIndex()const -> const int {
		return look_ahead_index_;	
	}

	auto SetLookAheadIndex(int look_ahead_index) -> void {
		look_ahead_index_ = look_ahead_index;	
	}

	auto MarkIndex() -> void {
		marker_list_.push_back(look_ahead_index_);
	}

	auto ReturnToLastMarkedIndex() -> void {
		look_ahead_index_ = marker_list_.back();
		marker_list_.pop_back();	
	} 

	auto IsSpeculating()const -> const bool {
		return !marker_list_.empty();	
	}

	auto LookAheadToken(unsigned int index) -> const Token {
		Synchronize(index);
		assert(look_ahead_index_+index-1 < look_ahead_token_list_.size());
		return look_ahead_token_list_[look_ahead_index_+index-1];	
	}

	auto Match(const TokenType& type) -> const Token {
		const auto token = LookAheadToken(1);
		std::ostringstream token_oss, type_oss;
		token_outputter_(token_oss, token);
		token_type_outputter_(type_oss, type);
		if(is_token_type_same_decider_(token, type)){
			DebugPrint("Match: "+token_oss.str()+" is "+type_oss.str());
			Consume();	
		}
		else {
			DebugPrint("MatchError: expecting "+type_oss.str()
				+" but "+token_oss.str());
			throw SyntaxError("SyntaxError: invalid syntax.");	
		}
		return token;
	}

private:
    BasicTokenBuffer(const NextTokenGetter& next_token_getter, 
				const IsTokenTypeSameDecider& is_token_type_same_decider,
				const TokenOutputter& token_outputter,
				const TokenTypeOutputter& token_type_outputter) :
			next_token_getter_(next_token_getter),
			is_token_type_same_decider_(is_token_type_same_decider),
			token_outputter_(token_outputter),
			token_type_outputter_(token_type_outputter),
			marker_list_(),
			look_ahead_token_list_(),
			look_ahead_index_(0){
	} 

	auto Synchronize(int index) -> void {
		int short_num = 
			look_ahead_index_+index-1 
				- (static_cast<int>(look_ahead_token_list_.size())-1);
		if(short_num > 0){
			for(int i = 0; i < short_num; ++i){
				look_ahead_token_list_.push_back(next_token_getter_());
			}	
		}
	}

	auto Consume() -> void {
		++look_ahead_index_;
		if(look_ahead_index_ == look_ahead_token_list_.size() && !IsSpeculating()){
			look_ahead_index_ = 0;
			look_ahead_token_list_.clear();
		}
		Synchronize(1);
	}

	NextTokenGetter next_token_getter_;
	IsTokenTypeSameDecider is_token_type_same_decider_;
	TokenOutputter token_outputter_;
	TokenTypeOutputter token_type_outputter_;

	std::vector<int> marker_list_;
	std::vector<Token> look_ahead_token_list_;
	unsigned int look_ahead_index_;
};
}

