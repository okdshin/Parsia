#pragma once
//TokenBuffer:20130611
#include <iostream>
#include <memory>
#include <functional>
#include <vector>
#include "Token.h"

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

class TokenBuffer{
public:
	using Ptr = std::shared_ptr<TokenBuffer>;
	using NextTokenGetter = std::function<Token ()>;

	static auto Create(const NextTokenGetter& next_token_getter) -> Ptr {
		return Ptr(new TokenBuffer(next_token_getter));	
	}

	auto DebugPrint(const std::string& text) -> void {
#ifdef PARSIA_TOKEN_BUFFER_DEBUG_ON
		if(IsSpeculating()){
			std::cout << "Speculating: " << text << std::endl;
		}
		else {
			std::cout << text << std::endl;	
		}
		std::cout << "\t";
		for(unsigned int i = 0; i < look_ahead_token_list_.size(); ++i){
			if(i == look_ahead_index_){
				std::cout << "*"; 
			}
			std::cout << "[" 
				<< look_ahead_token_list_.at(i).GetWord().ToString() << "] ";	
		}
		
		std::cout << "\n" << std::endl;
#endif
	}

	auto GetLookAheadIndex()const -> int {
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

	auto IsSpeculating() -> bool {
		return !marker_list_.empty();	
	}

	auto LookAheadToken(unsigned int index) -> Token {
		Synchronize(index);
		return look_ahead_token_list_.at(look_ahead_index_+index-1);	
	}

	auto LookAheadTokenType(unsigned int index) -> TokenType {
		return LookAheadToken(index).GetType().ToString();
	}

	auto Match(const TokenType& type) -> Token {
		const auto token = LookAheadToken(1);
		if(LookAheadTokenType(1) == type){
			DebugPrint("Match: "+LookAheadTokenType(1).ToString()
				+" "+LookAheadToken(1).GetWord().ToString());
			Consume();	
		}
		else {
			DebugPrint("MatchError: expecting "+type.ToString()
				+" but "+LookAheadTokenType(1).ToString()
				+" "+LookAheadToken(1).GetWord().ToString());
			throw SyntaxError("SyntaxError: invalid syntax.: "+type.ToString());	
		}
		return token;
	}

private:
    TokenBuffer(const NextTokenGetter& next_token_getter) : 
			next_token_getter_(next_token_getter),
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
	std::vector<int> marker_list_;
	std::vector<Token> look_ahead_token_list_;
	unsigned int look_ahead_index_;
};
}

