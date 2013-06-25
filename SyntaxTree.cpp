#ifdef SYNTAXTREE_UNIT_TEST
#include "SyntaxTree.h"
#include <iostream>

using namespace parsia;

int main(int argc, char* argv[])
{
	auto child1 = SyntaxTree::Create(Token(lexia::TokenType::IDENTIFIER(), lexia::Word("a")));
	auto child2  = SyntaxTree::Create(Token(lexia::TokenType::IDENTIFIER(), lexia::Word("b")));
	auto child3  = SyntaxTree::Create(Token(lexia::TokenType::IDENTIFIER(), lexia::Word("c")));
	auto root = SyntaxTree::Create(Token(lexia::TokenType::PLUS(), lexia::Word("+")));
	root->AddChildNode(child1);
	root->AddChildNode(child2);
	child1->AddChildNode(child3);
	std::cout << root->ToString() << std::endl;
    return 0;
}

#endif
