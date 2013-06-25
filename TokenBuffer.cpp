#ifdef TOKENBUFFER_UNIT_TEST
#include "TokenBuffer.h"
#include <iostream>
#include <fstream>
#include <streambuf>

using namespace parsia;

int main(int argc, char* argv[])
{
	try{
		auto token_buffer = TokenBuffer::Create([]() -> const Token {
				return Token(TokenType("TEST"), 
					Word("test"));
			});
		token_buffer->Match(TokenType("TEST"));
		token_buffer->Match(TokenType("TEST?"));
	}
	catch(const SyntaxError& e){
		std::cout << e.what() << std::endl;
	}
    return 0;
}

#endif
