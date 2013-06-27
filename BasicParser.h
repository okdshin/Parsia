#pragma once
//BasicParser:20130611
#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include "TokenBuffer.h"
#include "BasicSyntaxRule.h"

namespace parsia
{
template<class RuleReturnType>
class BasicParser {
public:
	using SyntaxRule = BasicSyntaxRule<RuleReturnType>;
	BasicParser() : token_buffer_(), syntax_rule_dict_(){}

	auto InitTokenBuffer(
			const TokenBuffer::NextTokenGetter& next_token_getter) -> void {
		token_buffer_ = TokenBuffer::Create(next_token_getter);
	}

	auto DefineSyntaxRule(const std::string& rule_name) -> const typename SyntaxRule::Ptr {
		assert("DuplicateDefinition" 
			&& syntax_rule_dict_.find(rule_name) == syntax_rule_dict_.end());
		syntax_rule_dict_.insert(std::pair<std::string, const typename SyntaxRule::Ptr>(
			rule_name,SyntaxRule::Create()));
		return syntax_rule_dict_.find(rule_name)->second;
	}

	auto ProcessRule(const std::string& rule_name)const -> const RuleReturnType {
		assert("No such name syntax rule! "
			&& syntax_rule_dict_.find(rule_name) != syntax_rule_dict_.end());
		return syntax_rule_dict_.find(rule_name)->second->ProcessRule(
			token_buffer_, 
			typename SyntaxRule::RuleProcessor([this](
					const std::string& rule_name) -> const RuleReturnType{
				return ProcessRule(rule_name);
			}));
	}

private:
	using SyntaxRuleDict = std::map<std::string, const typename SyntaxRule::Ptr>; 
	
	TokenBuffer::Ptr token_buffer_;
	SyntaxRuleDict syntax_rule_dict_;

};
}

