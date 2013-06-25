#pragma once
//SyntaxTree:20130614
#include <iostream>
#include <string>
#include <vector>
#include "Lexer.h"

namespace parsia
{
using Token = lexia::Token;

auto ToString(const Token& token) -> std::string {
	/*
	return "[" + token.GetType().ToString() 
		+ ":" + token.GetWord().ToString() + "]";
	*/
	return token.GetWord().ToString();
}

class SyntaxTree{
public:
	using Ptr = std::shared_ptr<SyntaxTree>;

	static auto Create(const Token& token) -> Ptr {
		return Ptr(new SyntaxTree(token));		
	}

	auto AddChild(const SyntaxTree::Ptr& child_node) -> void {
		child_tree_list_.push_back(child_node);
	}

	auto IsNil() -> bool {
		return token_.IsUninitialized();
	}

	auto ToString() -> std::string {
		if(child_tree_list_.empty()){
			return parsia::ToString(token_);
		}
		std::vector<std::string> str_buffer;
		if(!IsNil()){
			str_buffer.push_back("(");
			str_buffer.push_back(parsia::ToString(token_));
			str_buffer.push_back(" ");	
		}
		for(unsigned int i = 0; i < child_tree_list_.size(); ++i){
			if(i > 0){
				str_buffer.push_back(" ");	
			}
			str_buffer.push_back(child_tree_list_.at(i)->ToString());
		}
		if(!IsNil()){
			str_buffer.push_back(")");
		}
		std::string result;
		for(const auto& str : str_buffer){
			result = result + str;
		}
		return result;
	}
private:
    SyntaxTree() : token_(), child_tree_list_(){}
    SyntaxTree(const Token& token) : token_(token), child_tree_list_() {}

	Token token_;
	std::vector<SyntaxTree::Ptr> child_tree_list_;

};
}

