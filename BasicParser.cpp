#ifdef BASICPARSER_UNIT_TEST
#define PARSIA_TOKEN_BUFFER_DEBUG_ON
#include "BasicParser.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include "Token.h"

using namespace parsia;
using ReturnType = int;
using TokenBuffer = BasicTokenBuffer<Token, TokenType>;
using SyntaxRule = BasicSyntaxRule<Token, TokenType, ReturnType>;
using Parser = BasicParser<Token, TokenType, ReturnType>;

int main(int argc, char* argv[])
{
	Parser parser;
	parser.InitTokenBuffer(
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
	parser.DefineSyntaxRule("test_rule")
		->AddChoice([](const SyntaxRule::TokenMatcher& matcher, 
				const SyntaxRule::AheadTokenLooker& looker, 
				const SyntaxRule::RuleProcessor& processor,
				const SyntaxRule::IsSpeculatingDecider& decider) -> ReturnType {
			matcher(TokenType("TEST"));
			return 1;
		});

	std::cout << parser.ProcessRule("test_rule") << std::endl;

	return 0;
}

#endif
