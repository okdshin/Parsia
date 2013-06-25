#ifdef SYNTAXTREE_UNIT_TEST
#include "SyntaxTree.h"
#include <iostream>

using namespace parsia::sample;

int main(int argc, char* argv[])
{
	auto child1 = SyntaxTree::Create(Token(lexia::TokenType::IDENTIFIER(), lexia::Word("a")));
	auto child2  = SyntaxTree::Create(Token(lexia::TokenType::IDENTIFIER(), lexia::Word("b")));
	auto child3  = SyntaxTree::Create(Token(lexia::TokenType::IDENTIFIER(), lexia::Word("c")));
	auto root = SyntaxTree::Create(Token(lexia::TokenType::PLUS(), lexia::Word("+")));
	root->AddChild(child1);
	root->AddChild(child2);
	child1->AddChild(child3);
	std::cout << root->ToString() << std::endl;
    return 0;
}

#endif
