#ifdef BASICPARSER_UNIT_TEST
#include "BasicParser.h"
#include <iostream>
#include <fstream>
#include <streambuf>

using namespace parsia;
using Parser = BasicParser<SyntaxTree::Ptr>;

int main(int argc, char* argv[])
{
	std::ifstream ifs("test1.tc");
	std::string code(
		(std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	std::cout << code << std::endl;
	lexia::Lexer lexer(code);
	Parser parser(lexer);

	SyntaxTree::Ptr root;

	parser.DefineSyntaxRule("program")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			root = cons;
			cons->AddChild(processor("external_declaration"));	
			while(buffer->LookAheadTokenType(1) 
					!= lexia::TokenType::LEXIA_EOF_TOKEN_TYPE()){
				cons->AddChild(processor("external_declaration"));	
			}
			return cons;
		}));

	parser.DefineSyntaxRule("external_declaration")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##external_declaration1");
			return processor("declaration");
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##external_declaration2");
			return processor("function_definition");
		}));

	parser.DefineSyntaxRule("declaration")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##declaration");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::INT())));
			cons->AddChild(processor("declarator_list"));
			buffer->Match(lexia::TokenType::SEMICOLON());
			return cons;
		}));

	parser.DefineSyntaxRule("declarator_list")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##declarator_list");
			auto dec = processor("declarator");
			if(buffer->LookAheadTokenType(1) 
					!= lexia::TokenType::COMMA()){
				return dec;
			}
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(dec);
			while(buffer->LookAheadTokenType(1) 
					== lexia::TokenType::COMMA()){
				buffer->Match(lexia::TokenType::COMMA());
				cons->AddChild(processor("declarator"));
			}
			return cons;
		}));	

	parser.DefineSyntaxRule("declarator")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##declarator");
			return parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::IDENTIFIER()));	
		}));

	parser.DefineSyntaxRule("function_definition")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##function_definition1");
			auto cons1 = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			auto cons2 = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons1->AddChild(cons2);
			cons2->AddChild(parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::INT())));
			cons2->AddChild(processor("declarator"));
			buffer->Match(lexia::TokenType::LEFT_PARENTHESIS());
			buffer->Match(lexia::TokenType::RIGHT_PARENTHESIS());
			cons1->AddChild(processor("compound_statement"));
			return cons1;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##function_definition2");
			auto cons1 = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			auto cons2 = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons1->AddChild(cons2);
			cons2->AddChild(parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::INT())));
			cons2->AddChild(processor("declarator"));
			buffer->Match(lexia::TokenType::LEFT_PARENTHESIS());
			cons1->AddChild(processor("parameter_type_list"));
			buffer->Match(lexia::TokenType::RIGHT_PARENTHESIS());
			cons1->AddChild(processor("compound_statement"));
			return cons1;
		}))
		;	

	parser.DefineSyntaxRule("parameter_type_list")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##parameter_type_list");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(processor("parameter_declaration"));
			while(buffer->LookAheadTokenType(1) 
					== lexia::TokenType::COMMA()){
				buffer->Match(lexia::TokenType::COMMA());
				cons->AddChild(processor("parameter_declaration"));
			}
			return cons;
		}));

	parser.DefineSyntaxRule("parameter_declaration")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##parameter_declaration");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::INT())));
			cons->AddChild(processor("declarator"));
			return cons;
		}));

	parser.DefineSyntaxRule("statement")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##statement1");
			buffer->Match(lexia::TokenType::SEMICOLON());
			return parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##statement2");
			auto exp = processor("expression");
			buffer->Match(lexia::TokenType::SEMICOLON());
			return exp;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##statement3");
			return processor("compound_statement");
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##statement4");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::IF())));
			buffer->Match(lexia::TokenType::LEFT_PARENTHESIS());
			cons->AddChild(processor("expression"));
			buffer->Match(lexia::TokenType::RIGHT_PARENTHESIS());
			cons->AddChild(processor("statement"));
			buffer->Match(lexia::TokenType::ELSE());
			cons->AddChild(processor("statement"));
			return cons;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##statement5");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::IF())));
			buffer->Match(lexia::TokenType::LEFT_PARENTHESIS());
			cons->AddChild(processor("expression"));
			buffer->Match(lexia::TokenType::RIGHT_PARENTHESIS());
			cons->AddChild(processor("statement"));
			return cons;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##statement6");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::WHILE())));
			buffer->Match(lexia::TokenType::LEFT_PARENTHESIS());
			cons->AddChild(processor("expression"));
			buffer->Match(lexia::TokenType::RIGHT_PARENTHESIS());
			cons->AddChild(processor("statement"));
			return cons;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##statement7");
			auto ret = parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::RETURN()));
			buffer->Match(lexia::TokenType::SEMICOLON());
			return ret;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##statement8");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::RETURN())));
			cons->AddChild(processor("expression"));
			buffer->Match(lexia::TokenType::SEMICOLON());
			return cons;
		}));

	parser.DefineSyntaxRule("compound_statement")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##compound_statement1");
			buffer->Match(lexia::TokenType::LEFT_BRACE());
			buffer->Match(lexia::TokenType::RIGHT_BRACE());
			return parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##compound_statement2");
			buffer->Match(lexia::TokenType::LEFT_BRACE());
			auto dec = processor("declaration_list");
			buffer->Match(lexia::TokenType::RIGHT_BRACE());
			return dec;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##compound_statement3");
			buffer->Match(lexia::TokenType::LEFT_BRACE());
			auto sta = processor("statement_list");
			buffer->Match(lexia::TokenType::RIGHT_BRACE());
			return sta;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##compound_statement4");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			buffer->Match(lexia::TokenType::LEFT_BRACE());
			cons->AddChild(processor("declaration_list"));
			cons->AddChild(processor("statement_list"));
			buffer->Match(lexia::TokenType::RIGHT_BRACE());
			return cons;
		}));
	
	parser.DefineSyntaxRule("declaration_list")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##declaration_list");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(processor("declaration"));
			while(buffer->LookAheadTokenType(1) == lexia::TokenType::INT()){
				cons->AddChild(processor("declaration"));
			}
			return cons;
		}));

	parser.DefineSyntaxRule("statement_list")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##statement_list");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(processor("statement"));
			while(buffer->LookAheadTokenType(1) 
					== lexia::TokenType::SEMICOLON() || 
					buffer->LookAheadTokenType(1) 
						== lexia::TokenType::IDENTIFIER() || 
					buffer->LookAheadTokenType(1)
						== lexia::TokenType::MINUS() || 
					buffer->LookAheadTokenType(1) 
						== lexia::TokenType::LEFT_BRACE() || 
					buffer->LookAheadTokenType(1)
						== lexia::TokenType::IF() || 
					buffer->LookAheadTokenType(1)
						== lexia::TokenType::WHILE() || 
					buffer->LookAheadTokenType(1)
						== lexia::TokenType::RETURN()){
				cons->AddChild(processor("statement"));
			}
			return cons;
		}))
		;
	
	parser.DefineSyntaxRule("expression")
		->AddChoice(Parser::SyntaxRule::Choice([](
				const TokenBuffer::Ptr& buffer,
				const Parser::SyntaxRule::RuleProcessor& processor)
				-> const SyntaxTree::Ptr { 
			buffer->DebugPrint("##expression");
			auto ass = processor("assign_expression");
			if(buffer->LookAheadTokenType(1) != lexia::TokenType::COMMA()){
				return ass;
			}
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(ass);
			while(buffer->LookAheadTokenType(1) 
					== lexia::TokenType::COMMA()){
				buffer->Match(lexia::TokenType::COMMA());
				cons->AddChild(processor("assign_expression"));
			}	
			return cons;
		}))
		;
	
	parser.DefineSyntaxRule("assign_expression")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##assign_expression1");
			auto left_id = parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::IDENTIFIER()));
			auto eq = parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::EQUAL()));
			auto right_exp = processor("assign_expression");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(eq);
			cons->AddChild(left_id);
			cons->AddChild(right_exp);
			return cons;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##assign_expression2");
			return processor("logical_or_expression");
		}));

	parser.DefineSyntaxRule("logical_or_expression")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##logical_or_expression");
			auto first_exp = processor("logical_and_expression");
			if(buffer->LookAheadTokenType(1) != lexia::TokenType::OR()){
				return first_exp;
			}
			auto outer_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			auto inner_cons = outer_cons; 
			auto before_exp = first_exp;
			while(buffer->LookAheadTokenType(1) == lexia::TokenType::OR()){
				inner_cons->AddChild(parsia::SyntaxTree::Create(
					buffer->Match(lexia::TokenType::OR())));
				inner_cons->AddChild(before_exp);
				before_exp = processor("logical_and_expression");
				if(buffer->LookAheadTokenType(1) == lexia::TokenType::OR()){
					auto new_inner_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
					inner_cons->AddChild(new_inner_cons);
					inner_cons = new_inner_cons;
				}
			}	
			inner_cons->AddChild(before_exp);
			return outer_cons;
		}));

	parser.DefineSyntaxRule("logical_and_expression")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##logical_and_expression");
			auto first_exp = processor("equality_expression");
			if(buffer->LookAheadTokenType(1) != lexia::TokenType::AND()){
				return first_exp;
			}
			auto outer_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			auto inner_cons = outer_cons; 
			auto before_exp = first_exp;
			while(buffer->LookAheadTokenType(1) == lexia::TokenType::AND()){
				inner_cons->AddChild(parsia::SyntaxTree::Create(
					buffer->Match(lexia::TokenType::AND())));
				inner_cons->AddChild(before_exp);
				before_exp = processor("equality_expression");
				if(buffer->LookAheadTokenType(1) == lexia::TokenType::AND()){
					auto new_inner_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
					inner_cons->AddChild(new_inner_cons);
					inner_cons = new_inner_cons;
				}
			}	
			inner_cons->AddChild(before_exp);
			return outer_cons;
		}));

	parser.DefineSyntaxRule("equality_expression")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##equality_expression");
			auto first_exp = processor("relational_expression");
			if(buffer->LookAheadTokenType(1) != lexia::TokenType::EQUALEQUAL() 
					&& buffer->LookAheadTokenType(1) != lexia::TokenType::NOT_EQUAL()){
				return first_exp;
			}
			auto outer_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			auto inner_cons = outer_cons; 
			auto before_exp = first_exp;
			while(true){
				inner_cons->AddChild(parsia::SyntaxTree::Create(
					buffer->Match(buffer->LookAheadTokenType(1))));
				inner_cons->AddChild(before_exp);
				before_exp = processor("relational_expression");
				if(buffer->LookAheadTokenType(1) == lexia::TokenType::EQUALEQUAL()
						|| buffer->LookAheadTokenType(1) == lexia::TokenType::NOT_EQUAL()){
					auto new_inner_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
					inner_cons->AddChild(new_inner_cons);
					inner_cons = new_inner_cons;
				}
				else {
					inner_cons->AddChild(before_exp);
					break;	
				}
			}	
			return outer_cons;
		}));

	parser.DefineSyntaxRule("relational_expression")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##relational_expression");
			auto first_exp = processor("add_expression");
			if(buffer->LookAheadTokenType(1) != lexia::TokenType::LOWER_THAN() 
					&& buffer->LookAheadTokenType(1) != lexia::TokenType::HIGHER_THAN()
					&& buffer->LookAheadTokenType(1) != lexia::TokenType::LOWER_THAN()
					&& buffer->LookAheadTokenType(1) != lexia::TokenType::HIGHER_THAN()){
				return first_exp;
			}
			auto outer_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			auto inner_cons = outer_cons; 
			auto before_exp = first_exp;
			while(true){
				inner_cons->AddChild(parsia::SyntaxTree::Create(
					buffer->Match(buffer->LookAheadTokenType(1))));
				inner_cons->AddChild(before_exp);
				before_exp = processor("add_expression");
				if(buffer->LookAheadTokenType(1) == lexia::TokenType::LOWER_THAN() 
						|| buffer->LookAheadTokenType(1) == lexia::TokenType::HIGHER_THAN()
						|| buffer->LookAheadTokenType(1) == lexia::TokenType::LOWER_THAN()
						|| buffer->LookAheadTokenType(1) == lexia::TokenType::HIGHER_THAN()){
					auto new_inner_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
					inner_cons->AddChild(new_inner_cons);
					inner_cons = new_inner_cons;
				}
				else {
					inner_cons->AddChild(before_exp);
					break;	
				}
			}	
			return outer_cons;
		}));

	parser.DefineSyntaxRule("add_expression")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##add_expression");
			auto first_exp = processor("multiply_expression");
			if(buffer->LookAheadTokenType(1) != lexia::TokenType::PLUS() 
					&& buffer->LookAheadTokenType(1) != lexia::TokenType::MINUS()){
				return first_exp;
			}
			auto outer_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			auto inner_cons = outer_cons; 
			auto before_exp = first_exp;
			while(true){
				inner_cons->AddChild(parsia::SyntaxTree::Create(
					buffer->Match(buffer->LookAheadTokenType(1))));
				inner_cons->AddChild(before_exp);
				before_exp = processor("multiply_expression");
				if(buffer->LookAheadTokenType(1) == lexia::TokenType::PLUS() 
						|| buffer->LookAheadTokenType(1) == lexia::TokenType::MINUS()){
					auto new_inner_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
					inner_cons->AddChild(new_inner_cons);
					inner_cons = new_inner_cons;
				}
				else {
					inner_cons->AddChild(before_exp);
					break;	
				}
			}
			return outer_cons;
		}));
	
	parser.DefineSyntaxRule("multiply_expression")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##multiply_expression");
			auto first_exp = processor("unary_expression");
			if(buffer->LookAheadTokenType(1) != lexia::TokenType::MULTIPLY() 
					&& buffer->LookAheadTokenType(1) != lexia::TokenType::DIVIDE()){
				return first_exp;
			}
			auto outer_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			auto inner_cons = outer_cons; 
			auto before_exp = first_exp;
			while(true){
				inner_cons->AddChild(parsia::SyntaxTree::Create(
					buffer->Match(buffer->LookAheadTokenType(1))));
				inner_cons->AddChild(before_exp);
				before_exp = processor("unary_expression");
				if(buffer->LookAheadTokenType(1) == lexia::TokenType::MULTIPLY() 
						|| buffer->LookAheadTokenType(1) == lexia::TokenType::DIVIDE()){
					auto new_inner_cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
					inner_cons->AddChild(new_inner_cons);
					inner_cons = new_inner_cons;
				}
				else {
					inner_cons->AddChild(before_exp);
					break;	
				}
			}
			return outer_cons;
		}));
	
	parser.DefineSyntaxRule("unary_expression")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##unary_expression");
			if(buffer->LookAheadTokenType(1) == lexia::TokenType::MINUS()){
				auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
				cons->AddChild(parsia::SyntaxTree::Create(
					buffer->Match(lexia::TokenType::MINUS())));
				cons->AddChild(processor("unary_expression"));
				return cons;
			}
			else {
				return processor("postfix_expression");	
			}
		}));

	parser.DefineSyntaxRule("postfix_expression")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##postfix_expression1");
			auto id = parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::IDENTIFIER()));
			buffer->Match(lexia::TokenType::LEFT_PARENTHESIS());
			buffer->Match(lexia::TokenType::RIGHT_PARENTHESIS());
			return id;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##postfix_expression2");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(parsia::SyntaxTree::Create(
				buffer->Match(lexia::TokenType::IDENTIFIER())));
			buffer->Match(lexia::TokenType::LEFT_PARENTHESIS());
			cons->AddChild(processor("argument_expression_list"));
			buffer->Match(lexia::TokenType::RIGHT_PARENTHESIS());
			return cons;
		}))
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##postfix_expression3");
			return processor("primary_expression");	
		}));

	parser.DefineSyntaxRule("primary_expression")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##primary_expression");
			if(buffer->LookAheadTokenType(1) 
					== lexia::TokenType::IDENTIFIER()){	
				return parsia::SyntaxTree::Create(
					buffer->Match(lexia::TokenType::IDENTIFIER()));	
			}
			else if(buffer->LookAheadTokenType(1) 
					== lexia::TokenType::CONSTANT()){	
				return parsia::SyntaxTree::Create(
					buffer->Match(lexia::TokenType::CONSTANT()));	
			}
			else {
				buffer->Match(lexia::TokenType::LEFT_PARENTHESIS());	
				auto exp = processor("expression");
				buffer->Match(lexia::TokenType::RIGHT_PARENTHESIS());	
				return exp;
			}
		}));

	parser.DefineSyntaxRule("argument_expression_list")
		->AddChoice(Parser::SyntaxRule::Choice([&root](
				const TokenBuffer::Ptr& buffer, 
				const Parser::SyntaxRule::RuleProcessor& processor
				) -> const SyntaxTree::Ptr {
			buffer->DebugPrint("##argument_expression_list");
			auto cons = parsia::SyntaxTree::Create(lexia::Token::CONS_TOKEN());
			cons->AddChild(processor("assign_expression"));
			while(buffer->LookAheadTokenType(1) 
					== lexia::TokenType::COMMA()){ 
				buffer->Match(buffer->LookAheadTokenType(1));
				cons->AddChild(processor("assign_expression"));
			}	
			return cons;
		}));
	try{
		root = parser.ProcessRule("program");
	}
	catch(const SyntaxError& e){
		std::cout << e.what() << std::endl;
	}
	std::cout << root->ToString() << std::endl;
    return 0;
}

#endif
