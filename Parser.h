#pragma once
//Parser:20130611
#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include "Lexer.h"
#include "TokenBuffer.h"
#include "SyntaxRule.h"
#include "SyntaxTree.h"

namespace parsia
{
class Parser {
public:
	Parser(const lexia::Lexer& lexer) : 
		token_buffer_(TokenBuffer::Create(lexer)), syntax_rule_dict_(){}

	auto DefineSyntaxRule(const std::string& rule_name) -> const SyntaxRule::Ptr {
		assert("DuplicateDefinition" 
			&& syntax_rule_dict_.find(rule_name) == syntax_rule_dict_.end());
		syntax_rule_dict_.insert(std::pair<std::string, const SyntaxRule::Ptr>(
			rule_name, SyntaxRule::Create()));
		return syntax_rule_dict_.find(rule_name)->second;
	}

	auto ProcessRule(const std::string& rule_name)const -> const SyntaxTree::Ptr {
		assert("No such name syntax rule! "
			&& syntax_rule_dict_.find(rule_name) != syntax_rule_dict_.end());
		return syntax_rule_dict_.find(rule_name)->second->ProcessRule(
			token_buffer_, 
			SyntaxRule::RuleProcessor([this](
					const std::string& rule_name) -> const SyntaxTree::Ptr{
				return ProcessRule(rule_name);
			}));
	}

private:
	using SyntaxRuleDict = std::map<std::string, SyntaxRule::Ptr>; 
	
	const TokenBuffer::Ptr token_buffer_;
	SyntaxRuleDict syntax_rule_dict_;

};
}

