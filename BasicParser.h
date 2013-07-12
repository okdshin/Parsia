#pragma once
//BasicParser:20130611
#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include "BasicTokenBuffer.h"
#include "BasicSyntaxRule.h"

namespace parsia
{
template<class Token, class TokenType, class RuleReturnType>
class BasicParser {
public:
	using SyntaxRule = BasicSyntaxRule<Token, TokenType, RuleReturnType>;
	using TokenBuffer = BasicTokenBuffer<Token, TokenType>;
	BasicParser() : token_buffer_(), syntax_rule_dict_(), 
		rule_processor_(typename SyntaxRule::RuleProcessor([this](
				const std::string& rule_name) -> const RuleReturnType{
			return this->ProcessRule(rule_name);
		})){}

	auto DebugPrint(const std::string& message)const -> void {
		token_buffer_->DebugPrint(message);	
	}

	auto InitTokenBuffer(
			const typename TokenBuffer::NextTokenGetter& next_token_getter,
			const typename TokenBuffer::IsTokenTypeSameDecider& decider,
			const typename TokenBuffer::TokenOutputter& token_outputter,
			const typename TokenBuffer::TokenTypeOutputter& token_type_outputter) -> void {
		token_buffer_ = TokenBuffer::Create(
			next_token_getter, decider, token_outputter, token_type_outputter);
	}

	auto DefineSyntaxRule(
			const std::string& rule_name) -> const typename SyntaxRule::Ptr {
		assert("Please call InitTokenBuffer before define syntax rule." 
			&& token_buffer_ != nullptr);
		assert("DuplicateDefinition" 
			&& syntax_rule_dict_.find(rule_name) == syntax_rule_dict_.end());
		const auto syntax_rule = SyntaxRule::Create(token_buffer_);
		syntax_rule_dict_.insert(
				std::pair<std::string, const typename SyntaxRule::Ptr>(
			rule_name, syntax_rule));
		return syntax_rule;
	}

	auto ProcessRule(const std::string& rule_name)const -> const RuleReturnType {
		assert("No such name syntax rule."
			&& syntax_rule_dict_.find(rule_name) != syntax_rule_dict_.end());
		return syntax_rule_dict_.find(rule_name)->second->ProcessRule(
			rule_processor_);
	}

private:
	using SyntaxRuleDict = std::map<std::string, const typename SyntaxRule::Ptr>; 
	
	typename TokenBuffer::Ptr token_buffer_;
	SyntaxRuleDict syntax_rule_dict_;
	typename SyntaxRule::RuleProcessor rule_processor_;

};
}

