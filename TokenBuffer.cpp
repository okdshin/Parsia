#ifdef TOKENBUFFER_UNIT_TEST
#include "TokenBuffer.h"
#include <iostream>
#include <fstream>
#include <streambuf>

using namespace parsia;

int main(int argc, char* argv[])
{
	try{
		std::ifstream ifs("test.tc");
		std::string code(
			(std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		std::cout << code << std::endl;
		lexia::Lexer lexer(code);
		auto token_buffer = TokenBuffer::Create(lexer);
	}
	catch(const SyntaxError& e){
		std::cout << e.what() << std::endl;
	}
    return 0;
}

#endif
