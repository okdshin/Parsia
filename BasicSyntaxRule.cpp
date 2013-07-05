#ifdef BASICSYNTAXRULE_UNIT_TEST
#include "BasicSyntaxRule.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Token.h"

using namespace parsia;
using TokenBuffer = BasicTokenBuffer<Token, TokenType>;
using SyntaxRule = BasicSyntaxRule<Token, TokenType, int>;
int main(int argc, char* argv[])
{
	auto token_buffer = TokenBuffer::Create(
		TokenBuffer::NextTokenGetter([]() -> const Token {
			return Token(TokenType("TEST"), Word("test"));
		}),
		TokenBuffer::IsTokenTypeSameDecider([](
				const Token& token, const TokenType& type) -> bool {
			return token.GetType() == type;
		}),
		TokenBuffer::TokenOutputter([](
				std::ostream& os, const Token& token) -> void {
			os << "[" << token.GetWord().ToString() << "]";
		}),
		TokenBuffer::TokenTypeOutputter([](
				std::ostream& os, const TokenType& type) -> void {
			os << "\"" << type.ToString() << "\"";
		})
	);
	auto syntax_rule = SyntaxRule::Create(token_buffer);
	syntax_rule->AddChoice([](const SyntaxRule::TokenMatcher& matcher, 
			const SyntaxRule::AheadTokenLooker& looker, 
			const SyntaxRule::RuleProcessor& processor,
			const SyntaxRule::IsSpeculatingDecider& is_speculating_decider){
		return 1;
	});
	syntax_rule->ProcessRule([](const std::string& rule_name){ return 1; });
    return 0;
}
#endif
