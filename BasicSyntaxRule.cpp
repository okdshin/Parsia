#ifdef BASICSYNTAXRULE_UNIT_TEST
#include "BasicSyntaxRule.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace parsia;

int main(int argc, char* argv[])
{
	std::ifstream ifs("test1.tc");
	std::string code(
		(std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	lexia::Lexer lexer(code);
	auto parser = Parser::Create(lexer);

	auto program = BasicSyntaxRule(parser); 
	auto external_declaration = BasicSyntaxRule(parser);
	auto declaration = BasicSyntaxRule(parser);
	auto function_definition = BasicSyntaxRule(parser);	
	auto declarator_list = BasicSyntaxRule(parser);
	auto declarator = BasicSyntaxRule(parser);
	auto parameter_type_list = BasicSyntaxRule(parser);
	auto parameter_declaration = BasicSyntaxRule(parser);
	auto statement = BasicSyntaxRule(parser);
	auto compound_statement = BasicSyntaxRule(parser);
	auto declaration_list = BasicSyntaxRule(parser);
	auto statement_list = BasicSyntaxRule(parser);
	auto expression = BasicSyntaxRule(parser);
	auto assign_expression = BasicSyntaxRule(parser);
	auto logical_or_expression = BasicSyntaxRule(parser);
	auto logical_and_expression = BasicSyntaxRule(parser);
	auto equality_expression = BasicSyntaxRule(parser);
	auto relational_expression = BasicSyntaxRule(parser);
	auto add_expression = BasicSyntaxRule(parser);
	auto multiply_expression = BasicSyntaxRule(parser);
	auto unary_expression = BasicSyntaxRule(parser);
	auto postfix_expression = BasicSyntaxRule(parser);
	auto primary_expression = BasicSyntaxRule(parser);
	auto argument_expression_list = BasicSyntaxRule(parser);
	
	program
		.AddChoice(
			BasicSyntaxRule::Choice([&]() -> void {
				parser->DebugPrint("##program");
				external_declaration();
				while(parser->LookAheadTokenType(1) 
						!= lexia::TokenType::LEXIA_EOF_TOKEN_TYPE()){
					external_declaration();	
				}
			}));
	external_declaration
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##external_declaration1");
				declaration();
			}))
		.AddChoice(BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##external_declaration2");
				function_definition();
			}));

	declaration
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##declaration");
				parser->Match(lexia::TokenType::INT());
				declarator_list();
				parser->Match(lexia::TokenType::SEMICOLON());
			}));

	declarator_list
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##declarator_list");
				declarator();
				while(parser->LookAheadTokenType(1) == lexia::TokenType::COMMA()){
					parser->Match(lexia::TokenType::COMMA());
					declarator();
				}
			}));	

	declarator
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##declarator");
				parser->Match(lexia::TokenType::IDENTIFIER());	
			}));

	function_definition
		.AddChoice(
			BasicSyntaxRule::Choice([&]() -> void {
				parser->DebugPrint("##function_definition1");
				parser->Match(lexia::TokenType::INT());
				declarator();
				parser->Match(lexia::TokenType::LEFT_PARENTHESIS());
				parser->Match(lexia::TokenType::RIGHT_PARENTHESIS());
				compound_statement();
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##function_definition2");
				parser->Match(lexia::TokenType::INT());
				declarator();
				parser->Match(lexia::TokenType::LEFT_PARENTHESIS());
				parameter_type_list();
				parser->Match(lexia::TokenType::RIGHT_PARENTHESIS());
				compound_statement();
			}));	

	parameter_type_list
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##parameter_type_list");
				parameter_declaration();
				while(parser->LookAheadTokenType(1) == lexia::TokenType::COMMA()){
					parser->Match(lexia::TokenType::COMMA());
					parameter_declaration();
				}
			}));

	parameter_declaration
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##parameter_declaration");
				parser->Match(lexia::TokenType::INT());
				declarator();
			}));

	statement
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##statement1");
				parser->Match(lexia::TokenType::SEMICOLON());
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##statement2");
				expression();
				parser->Match(lexia::TokenType::SEMICOLON());
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##statement3");
				compound_statement();
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##statement4");
				parser->Match(lexia::TokenType::IF());
				parser->Match(lexia::TokenType::LEFT_PARENTHESIS());
				expression();
				parser->Match(lexia::TokenType::RIGHT_PARENTHESIS());
				statement();
				parser->Match(lexia::TokenType::ELSE());
				statement();
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##statement5");
				parser->Match(lexia::TokenType::IF());
				parser->Match(lexia::TokenType::LEFT_PARENTHESIS());
				expression();
				parser->Match(lexia::TokenType::RIGHT_PARENTHESIS());
				statement();
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##statement6");
				parser->Match(lexia::TokenType::WHILE());
				parser->Match(lexia::TokenType::LEFT_PARENTHESIS());
				expression();
				parser->Match(lexia::TokenType::RIGHT_PARENTHESIS());
				statement();
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##statement7");
				parser->Match(lexia::TokenType::RETURN());
				parser->Match(lexia::TokenType::SEMICOLON());
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##statement8");
				parser->Match(lexia::TokenType::RETURN());
				expression();
				parser->Match(lexia::TokenType::SEMICOLON());
			}));

	compound_statement
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##compound_statement1");
				parser->Match(lexia::TokenType::LEFT_BRACE());
				parser->Match(lexia::TokenType::RIGHT_BRACE());
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##compound_statement2");
				parser->Match(lexia::TokenType::LEFT_BRACE());
				declaration_list();
				parser->Match(lexia::TokenType::RIGHT_BRACE());
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##compound_statement3");
				parser->Match(lexia::TokenType::LEFT_BRACE());
				statement_list();
				parser->Match(lexia::TokenType::RIGHT_BRACE());
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##compound_statement4");
				parser->Match(lexia::TokenType::LEFT_BRACE());
				declaration_list();
				statement_list();
				parser->Match(lexia::TokenType::RIGHT_BRACE());
			}));
	
	declaration_list
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##declaration_list");
				declaration();
				while(parser->LookAheadTokenType(1) == lexia::TokenType::INT()){
					declaration();
				}
			}));

	statement_list
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##statement_list");
				statement();
				while(parser->LookAheadTokenType(1) 
							== lexia::TokenType::SEMICOLON() || 
						parser->LookAheadTokenType(1) 
							== lexia::TokenType::IDENTIFIER() || 
						parser->LookAheadTokenType(1)
							== lexia::TokenType::MINUS() || 
						parser->LookAheadTokenType(1) 
							== lexia::TokenType::LEFT_BRACE() || 
						parser->LookAheadTokenType(1)
							== lexia::TokenType::IF() || 
						parser->LookAheadTokenType(1)
							== lexia::TokenType::WHILE() || 
						parser->LookAheadTokenType(1)
							== lexia::TokenType::RETURN()){
					statement();
				}
			}));

	expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##expression");
				assign_expression();
				while(parser->LookAheadTokenType(1) == lexia::TokenType::COMMA()){
					parser->Match(lexia::TokenType::COMMA());
					assign_expression();
				}	
			}));

	assign_expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##assign_expression1");
				parser->Match(lexia::TokenType::IDENTIFIER());
				parser->Match(lexia::TokenType::EQUAL());
				assign_expression();
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##assign_expression2");
				logical_or_expression();
			}));

	logical_or_expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##logical_or_expression");
				logical_and_expression();
				while(parser->LookAheadTokenType(1) == lexia::TokenType::OR()){
					parser->Match(lexia::TokenType::OR());
					logical_and_expression();
				}	
			}));

	logical_and_expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##logical_and_expression");
				equality_expression();
				while(parser->LookAheadTokenType(1) == lexia::TokenType::AND()){
					parser->Match(lexia::TokenType::AND());
					equality_expression();
				}	
			}));

	equality_expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##equality_expression");
				relational_expression();
				while(parser->LookAheadTokenType(1) == lexia::TokenType::EQUALEQUAL() 
						|| parser->LookAheadTokenType(1) 
							== lexia::TokenType::NOT_EQUAL()){
					parser->Match(parser->LookAheadTokenType(1));
					relational_expression();
				}
			}));

	relational_expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##relational_expression");
				add_expression();
				while(parser->LookAheadTokenType(1) == lexia::TokenType::LOWER_THAN() 
						|| parser->LookAheadTokenType(1) 
							== lexia::TokenType::HIGHER_THAN()
						|| parser->LookAheadTokenType(1) 
							== lexia::TokenType::LOWER_EQUAL()
						|| parser->LookAheadTokenType(1) 
							== lexia::TokenType::HIGHER_EQUAL()){
					parser->Match(parser->LookAheadTokenType(1));
					add_expression();
				}		
			}));

	add_expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##add_expression");
				multiply_expression();
				while(parser->LookAheadTokenType(1) == lexia::TokenType::PLUS() 
						|| parser->LookAheadTokenType(1) == lexia::TokenType::MINUS()){
					parser->Match(parser->LookAheadTokenType(1));
					multiply_expression();
				}
			}));
	
	multiply_expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##multiply_expression");
				unary_expression();
				while(parser->LookAheadTokenType(1) == lexia::TokenType::MULTIPLY() 
						|| parser->LookAheadTokenType(1) == lexia::TokenType::DIVIDE()){
					parser->Match(parser->LookAheadTokenType(1));
					unary_expression();
				}
			}));
	
	unary_expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##unary_expression");
				if(parser->LookAheadTokenType(1) == lexia::TokenType::MINUS()){
					parser->Match(lexia::TokenType::MINUS());
					unary_expression();
				}
				else {
					postfix_expression();	
				}
			}));

	postfix_expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##postfix_expression1");
				parser->Match(lexia::TokenType::IDENTIFIER());
				parser->Match(lexia::TokenType::LEFT_PARENTHESIS());
				parser->Match(lexia::TokenType::RIGHT_PARENTHESIS());
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##postfix_expression2");
				parser->Match(lexia::TokenType::IDENTIFIER());
				parser->Match(lexia::TokenType::LEFT_PARENTHESIS());
				argument_expression_list();
				parser->Match(lexia::TokenType::RIGHT_PARENTHESIS());
			}))
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##postfix_expression3");
				primary_expression();	
			}));

	primary_expression
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##primary_expression");
				if(parser->LookAheadTokenType(1) 
						== lexia::TokenType::IDENTIFIER()){	
					parser->Match(lexia::TokenType::IDENTIFIER());	
				}
				else if(parser->LookAheadTokenType(1) 
						== lexia::TokenType::CONSTANT()){	
					parser->Match(lexia::TokenType::CONSTANT());	
				}
				else {
					parser->Match(lexia::TokenType::LEFT_PARENTHESIS());	
					expression();
					parser->Match(lexia::TokenType::RIGHT_PARENTHESIS());	
				}
			}));

	argument_expression_list
		.AddChoice(
			BasicSyntaxRule::Choice([&](){
				parser->DebugPrint("##argument_expression_list");
				assign_expression();
				while(parser->LookAheadTokenType(1) 
						== lexia::TokenType::COMMA()){ 
					parser->Match(parser->LookAheadTokenType(1));
					assign_expression();
				}	
			}));

	try{
		program();
	}
	catch(const SyntaxError& e){
		std::cout << e.what() << std::endl;
	}

    return 0;
}
#endif
