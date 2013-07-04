#ifdef BASICTOKENBUFFER_UNIT_TEST
#define PARSIA_TOKEN_BUFFER_DEBUG_ON
#include "BasicTokenBuffer.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include "Token.h"


using namespace parsia;

using TokenBuffer = BasicTokenBuffer<Token, TokenType>;
int main(int argc, char* argv[])
{
	try{
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
		token_buffer->Match(TokenType("TEST"));
		token_buffer->Match(TokenType("TEST"));
		token_buffer->Match(TokenType("TEST"));
		token_buffer->Match(TokenType("TEST?"));
	}
	catch(const SyntaxError& e){
		std::cout << e.what() << std::endl;
	}
    return 0;
}

#endif
