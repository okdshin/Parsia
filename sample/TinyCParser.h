#pragma once
//TinyCParser:20130624
#define PARSIA_TOKEN_BUFFER_DEBUG_ON
#include <iostream>
#include "Lexia/Lexer.h"
#include "../BasicParser.h"
#include "BasicTree.h"
#include "Token.h"

#define PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS \
const LangParser::SyntaxRule::TokenMatcher& matcher,\
const LangParser::SyntaxRule::AheadTokenLooker& looker,\
const LangParser::SyntaxRule::RuleProcessor& processor,\
const LangParser::SyntaxRule::IsSpeculatingDecider& decider

namespace tiger_cat
{
using LangToken = lexia::Token;
using LangTokenType = lexia::TokenType;
using Ast = semantia::BasicTree<Token::Ptr>;
using LangParser = parsia::BasicParser<LangToken, LangTokenType, Ast::Ptr>;

auto CreateToken(const lexia::Token& lexia_token) -> const Token::Ptr {
	return Token::Create(TokenType(lexia_token.GetType().ToString()),
		Word(lexia_token.GetWord().ToString()));
}

auto CreateAst(const lexia::Token& lexia_token) -> const Ast::Ptr {
	return Ast::Create(CreateToken(lexia_token));	
}

auto GetType(const LangToken& token) -> const LangTokenType {
	return token.GetType();
}

auto IsTokenTypeSame(const LangToken& token, const LangTokenType& type) -> const bool {
	return GetType(token) == type;
}

class TinyCParser{
public:
    TinyCParser(const std::string& code) : 
		lexer_(code),
		lang_parser_(),
		ast_root_(){}

	auto Define() -> void {
		DefineLanguageSyntax();
	}

	auto InitLangTokenBuffer() -> void {
		lang_parser_.InitTokenBuffer(
			LangParser::TokenBuffer::NextTokenGetter([this]() -> const LangToken {
				return lexer_.GetNextToken();
			}),
			LangParser::TokenBuffer::IsTokenTypeSameDecider([](
					const LangToken& token, const LangTokenType& type) -> const bool {
				return token.GetType() == type;
			}),	
			LangParser::TokenBuffer::TokenOutputter([](
					std::ostream& os, const LangToken& token) -> void {
				os << "[" << token.GetWord().ToString() << "]";
			}),
			LangParser::TokenBuffer::TokenTypeOutputter([](
					std::ostream& os, const LangTokenType& type) -> void {
				os << "\"" << type.ToString() << "\"";
			})
		);
	}

	auto MakeAbstractSyntaxTree() -> const Ast::Ptr {
		ast_root_ = lang_parser_.ProcessRule("program");
		return ast_root_;
	}

private:
	lexia::Lexer lexer_;
	LangParser lang_parser_;
	Ast::Ptr ast_root_;

public:
	auto DefineLanguageSyntax() -> void {
		lang_parser_.DefineSyntaxRule("program")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr {
				lang_parser_.DebugPrint("##program");
				auto cons = CreateAst(lexia::Token::CONS_TOKEN());
				cons->AddChild(processor("external_declaration"));	
				while(!IsTokenTypeSame(looker(1), 
						lexia::TokenType::LEXIA_EOF_TOKEN_TYPE())){
					cons->AddChild(processor("external_declaration"));	
				}
				return cons;
			}));

		lang_parser_.DefineSyntaxRule("external_declaration")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##external_declaration1");
				return processor("declaration");
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##external_declaration2");
				return processor("function_definition");
			}));

		lang_parser_.DefineSyntaxRule("declaration")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##declaration");
				auto cons = 
					CreateAst(lexia::Token::VARIABLE_DECLARATION_TOKEN());
				cons->AddChild(CreateAst(matcher(lexia::TokenType::INT())));
				cons->AddChild(processor("declarator_list"));
				matcher(lexia::TokenType::SEMICOLON());
				return cons;
			}));

		lang_parser_.DefineSyntaxRule("declarator_list")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##declarator_list");
				auto dec = processor("declarator");
				if(!IsTokenTypeSame(looker(1), lexia::TokenType::COMMA())){
					return dec;
				}
				auto cons = CreateAst(lexia::Token::CONS_TOKEN());
				cons->AddChild(dec);
				while(IsTokenTypeSame(looker(1), lexia::TokenType::COMMA())){
					matcher(lexia::TokenType::COMMA());
					cons->AddChild(processor("declarator"));
				}
				return cons;
			}));	

		lang_parser_.DefineSyntaxRule("declarator")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##declarator");
				return CreateAst(matcher(lexia::TokenType::IDENTIFIER()));	
			}));

		lang_parser_.DefineSyntaxRule("function_definition")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##function_definition1");
				auto cons1 = 
					CreateAst(lexia::Token::FUNCTION_DECLARATION_TOKEN());
				auto cons2 = CreateAst(lexia::Token::CONS_TOKEN());
				cons1->AddChild(cons2);
				cons2->AddChild(CreateAst(matcher(lexia::TokenType::INT())));
				cons2->AddChild(processor("declarator"));
				matcher(lexia::TokenType::LEFT_PARENTHESIS());
				matcher(lexia::TokenType::RIGHT_PARENTHESIS());
				cons1->AddChild(processor("compound_statement"));
				return cons1;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##function_definition2");
				auto cons1 = 
					CreateAst(lexia::Token::FUNCTION_DECLARATION_TOKEN());
				auto cons2 = CreateAst(lexia::Token::CONS_TOKEN());
				cons1->AddChild(cons2);
				cons2->AddChild(CreateAst(matcher(lexia::TokenType::INT())));
				cons2->AddChild(processor("declarator"));
				matcher(lexia::TokenType::LEFT_PARENTHESIS());
				cons1->AddChild(processor("parameter_type_list"));
				matcher(lexia::TokenType::RIGHT_PARENTHESIS());
				cons1->AddChild(processor("compound_statement"));
				return cons1;
			}))
			;	

		lang_parser_.DefineSyntaxRule("parameter_type_list")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##parameter_type_list");
				auto cons = CreateAst(lexia::Token::CONS_TOKEN());
				cons->AddChild(processor("parameter_declaration"));
				while(IsTokenTypeSame(looker(1), lexia::TokenType::COMMA())){
					matcher(lexia::TokenType::COMMA());
					cons->AddChild(processor("parameter_declaration"));
				}
				return cons;
			}));

		lang_parser_.DefineSyntaxRule("parameter_declaration")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##parameter_declaration");
				auto cons = CreateAst(lexia::Token::PARAMETER_DECLARATION_TOKEN());
				cons->AddChild(CreateAst(matcher(lexia::TokenType::INT())));
				cons->AddChild(processor("declarator"));
				return cons;
			}));
		
		lang_parser_.DefineSyntaxRule("statement")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##statement1");
				matcher(lexia::TokenType::SEMICOLON());
				return CreateAst(lexia::Token::CONS_TOKEN());
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##statement2");
				auto exp = processor("expression");
				matcher(lexia::TokenType::SEMICOLON());
				return exp;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##statement3");
				return processor("compound_statement");
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##statement4");
				matcher(lexia::TokenType::IF());
				auto if_node = CreateAst(lexia::Token::IF_OF_IF_WITH_ELSE_TOKEN());
				matcher(lexia::TokenType::LEFT_PARENTHESIS());
				if_node->AddChild(processor("expression"));
				matcher(lexia::TokenType::RIGHT_PARENTHESIS());
				if_node->AddChild(processor("statement"));
				auto else_node = CreateAst(matcher(lexia::TokenType::ELSE()));
				else_node->AddChild(processor("statement"));
				auto cons = CreateAst(lexia::Token::IF_WITH_ELSE_TOKEN());
				cons->AddChild(if_node);
				cons->AddChild(else_node);
				return cons;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##statement5");
				auto cons = CreateAst(matcher(lexia::TokenType::IF()));
				matcher(lexia::TokenType::LEFT_PARENTHESIS());
				cons->AddChild(processor("expression"));
				matcher(lexia::TokenType::RIGHT_PARENTHESIS());
				cons->AddChild(processor("statement"));
				return cons;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##statement6");
				auto cons = CreateAst(matcher(lexia::TokenType::WHILE()));
				matcher(lexia::TokenType::LEFT_PARENTHESIS());
				cons->AddChild(processor("expression"));
				matcher(lexia::TokenType::RIGHT_PARENTHESIS());
				cons->AddChild(processor("statement"));
				return cons;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##statement7");
				auto ret = CreateAst(matcher(lexia::TokenType::RETURN()));
				matcher(lexia::TokenType::SEMICOLON());
				return ret;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##statement8");
				auto cons = CreateAst(matcher(lexia::TokenType::RETURN()));
				cons->AddChild(processor("expression"));
				matcher(lexia::TokenType::SEMICOLON());
				return cons;
			}));
		
		lang_parser_.DefineSyntaxRule("compound_statement")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##compound_statement1");
				matcher(lexia::TokenType::LEFT_BRACE());
				matcher(lexia::TokenType::RIGHT_BRACE());
				return CreateAst(lexia::Token::BLOCK_TOKEN());
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##compound_statement2");
				auto block = CreateAst(lexia::Token::BLOCK_TOKEN());
				matcher(lexia::TokenType::LEFT_BRACE());
				block->AddChild(processor("declaration_list"));
				matcher(lexia::TokenType::RIGHT_BRACE());
				return block;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##compound_statement3");
				auto block = CreateAst(lexia::Token::BLOCK_TOKEN());
				matcher(lexia::TokenType::LEFT_BRACE());
				block->AddChild(processor("statement_list"));
				matcher(lexia::TokenType::RIGHT_BRACE());
				return block;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##compound_statement4");
				auto block = CreateAst(lexia::Token::BLOCK_TOKEN());
				matcher(lexia::TokenType::LEFT_BRACE());
				block->AddChild(processor("declaration_list"));
				block->AddChild(processor("statement_list"));
				matcher(lexia::TokenType::RIGHT_BRACE());
				return block;
			}));

		lang_parser_.DefineSyntaxRule("declaration_list")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##declaration_list");
				auto cons = CreateAst(lexia::Token::CONS_TOKEN());
				cons->AddChild(processor("declaration"));
				while(IsTokenTypeSame(looker(1), lexia::TokenType::INT())){
					cons->AddChild(processor("declaration"));
				}
				return cons;
			}));

		lang_parser_.DefineSyntaxRule("statement_list")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##statement_list");
				auto cons = CreateAst(lexia::Token::CONS_TOKEN());
				cons->AddChild(processor("statement"));
				while(IsTokenTypeSame(looker(1), 
							lexia::TokenType::SEMICOLON()) || 
						IsTokenTypeSame(looker(1), 
							lexia::TokenType::IDENTIFIER()) || 
						IsTokenTypeSame(looker(1), 
							lexia::TokenType::MINUS()) || 
						IsTokenTypeSame(looker(1), 
							lexia::TokenType::LEFT_BRACE()) || 
						IsTokenTypeSame(looker(1), 
							lexia::TokenType::IF()) || 
						IsTokenTypeSame(looker(1), 
							lexia::TokenType::WHILE()) || 
						IsTokenTypeSame(looker(1), 
							lexia::TokenType::RETURN())){
					cons->AddChild(processor("statement"));
				}
				return cons;
			}))
			;
	
		lang_parser_.DefineSyntaxRule("expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##expression");
				auto ass = processor("assign_expression");
				if(!IsTokenTypeSame(looker(1), lexia::TokenType::COMMA())){
					return ass;
				}
				auto cons = CreateAst(lexia::Token::CONS_TOKEN());
				cons->AddChild(ass);
				while(IsTokenTypeSame(looker(1), lexia::TokenType::COMMA())){
					matcher(lexia::TokenType::COMMA());
					cons->AddChild(processor("assign_expression"));
				}	
				return cons;
			}))
			;
		
		lang_parser_.DefineSyntaxRule("assign_expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##assign_expression1");
				auto left_id = CreateAst(matcher(lexia::TokenType::IDENTIFIER()));
				matcher(lexia::TokenType::EQUAL());
				auto right_exp = processor("assign_expression");
				auto var_ref = CreateAst(lexia::Token::VARIABLE_REFERENCE_TOKEN());
				var_ref->AddChild(left_id);
				auto cons = CreateAst(lexia::Token::ASSIGN_TOKEN());
				cons->AddChild(var_ref);
				cons->AddChild(right_exp);
				return cons;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##assign_expression2");
				return processor("logical_or_expression");
			}));

		lang_parser_.DefineSyntaxRule("logical_or_expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##logical_or_expression");
				auto first_exp = processor("logical_and_expression");
				if(!IsTokenTypeSame(looker(1), lexia::TokenType::OR())){
					return first_exp;
				}
				auto outer_cons = CreateAst(lexia::Token::LOGICAL_OR_TOKEN());
				auto inner_cons = outer_cons; 
				auto before_exp = first_exp;
				while(IsTokenTypeSame(looker(1), lexia::TokenType::OR())){
					matcher(lexia::TokenType::OR());
					inner_cons->AddChild(before_exp);
					before_exp = processor("logical_and_expression");
					if(IsTokenTypeSame(looker(1), lexia::TokenType::OR())){
						auto new_inner_cons = 
							CreateAst(lexia::Token::LOGICAL_OR_TOKEN());
						inner_cons->AddChild(new_inner_cons);
						inner_cons = new_inner_cons;
					}
				}	
				inner_cons->AddChild(before_exp);
				return outer_cons;
			}));

		lang_parser_.DefineSyntaxRule("logical_and_expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##logical_and_expression");
				auto first_exp = processor("equality_expression");
				if(!IsTokenTypeSame(looker(1), lexia::TokenType::AND())){
					return first_exp;
				}
				auto outer_cons = CreateAst(lexia::Token::LOGICAL_AND_TOKEN());
				auto inner_cons = outer_cons; 
				auto before_exp = first_exp;
				while(IsTokenTypeSame(looker(1), lexia::TokenType::AND())){
					matcher(lexia::TokenType::AND());
					inner_cons->AddChild(before_exp);
					before_exp = processor("equality_expression");
					if(IsTokenTypeSame(looker(1), lexia::TokenType::AND())){
						auto new_inner_cons = 
							CreateAst(lexia::Token::LOGICAL_AND_TOKEN());
						inner_cons->AddChild(new_inner_cons);
						inner_cons = new_inner_cons;
					}
				}	
				inner_cons->AddChild(before_exp);
				return outer_cons;
			}));

		lang_parser_.DefineSyntaxRule("equality_expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##equality_expression");
				auto first_exp = processor("relational_expression");
				Ast::Ptr outer_cons;
				if(IsTokenTypeSame(looker(1), lexia::TokenType::EQUALEQUAL())){
					outer_cons = CreateAst(lexia::Token::EQUALITY_TOKEN());	
				}else
				if(IsTokenTypeSame(looker(1), lexia::TokenType::NOT_EQUAL())){
					outer_cons = CreateAst(lexia::Token::NOT_EQUALITY_TOKEN());
				}
				else{
					return first_exp;	
				}
				outer_cons->AddChild(first_exp);
				matcher(looker(1).GetType());
				outer_cons->AddChild(processor("relational_expression"));
				return outer_cons;
			}));

		lang_parser_.DefineSyntaxRule("relational_expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##relational_expression");
				auto first_exp = processor("add_expression");
				Ast::Ptr outer_cons;
				if(IsTokenTypeSame(looker(1), lexia::TokenType::LOWER_THAN())){	
					outer_cons = CreateAst(
						lexia::Token::RELATIONAL_LOWER_THAN_TOKEN());
				}else
				if(IsTokenTypeSame(looker(1), lexia::TokenType::HIGHER_THAN())){
					outer_cons = CreateAst(
						lexia::Token::RELATIONAL_HIGHER_THAN_TOKEN());
				}else
				if(IsTokenTypeSame(looker(1), lexia::TokenType::LOWER_EQUAL())){
					outer_cons = CreateAst(
						lexia::Token::RELATIONAL_LOWER_EQUAL_TOKEN());
				}else
				if(IsTokenTypeSame(looker(1), lexia::TokenType::HIGHER_EQUAL())){
					outer_cons = CreateAst(
						lexia::Token::RELATIONAL_HIGHER_EQUAL_TOKEN());
				}
				else {
					return first_exp;	
				}
				outer_cons->AddChild(first_exp);
				matcher(looker(1).GetType());
				outer_cons->AddChild(processor("add_expression"));
				
				return outer_cons;
			}));
		
		lang_parser_.DefineSyntaxRule("add_expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##add_expression");
				auto first_exp = processor("multiply_expression");
				auto is_add = [](const LangToken& token) -> const bool {
						return IsTokenTypeSame(token, lexia::TokenType::PLUS());
					};
				auto is_sub = [](const LangToken& token) -> const bool {
						return IsTokenTypeSame(token, lexia::TokenType::MINUS());
					};
				auto parent = first_exp;
				while(true){
					Ast::Ptr new_parent;
					if(is_add(looker(1))){
						new_parent = CreateAst(lexia::Token::ADD_TOKEN());
					}else
					if(is_sub(looker(1))){
						new_parent = CreateAst(lexia::Token::SUB_TOKEN());
					}else{
						break;	
					}
					matcher(GetType(looker(1)));
					new_parent->AddChild(parent);
					new_parent->AddChild(processor("multiply_expression"));
					parent = new_parent;
				}
				return parent;
			}));

		lang_parser_.DefineSyntaxRule("multiply_expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##multiply_expression");
				auto first_exp = processor("unary_expression");
				auto is_mul = [](const LangToken& token) -> const bool {
						return IsTokenTypeSame(token, 
							lexia::TokenType::ASTERISK());
					};
				auto is_div = [](const LangToken& token) -> const bool {
						return IsTokenTypeSame(token, lexia::TokenType::SLASH());
					};
				auto is_rem = [](const LangToken& token) -> const bool {
						return IsTokenTypeSame(token, lexia::TokenType::PERCENT());
					};
				auto parent = first_exp;
				while(true){
					Ast::Ptr new_parent;
					if(is_mul(looker(1))){
						new_parent = CreateAst(lexia::Token::MUL_TOKEN());
					}else
					if(is_div(looker(1))){
						new_parent = CreateAst(lexia::Token::DIV_TOKEN());
					}else
					if(is_rem(looker(1))){
						new_parent = CreateAst(lexia::Token::REM_TOKEN());
					}else{
						break;	
					}
					matcher(GetType(looker(1)));
					new_parent->AddChild(parent);
					new_parent->AddChild(processor("unary_expression"));
					parent = new_parent;
				}
				return parent;
			}));

		lang_parser_.DefineSyntaxRule("unary_expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##unary_expression");
				if(IsTokenTypeSame(looker(1), lexia::TokenType::MINUS())){
					matcher(lexia::TokenType::MINUS());
					auto cons = CreateAst(lexia::Token::UNARY_MINUS_TOKEN());
					cons->AddChild(processor("unary_expression"));
					return cons;
				}
				else {
					return processor("postfix_expression");	
				}
			}));

		lang_parser_.DefineSyntaxRule("postfix_expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##postfix_expression1");
				auto func_call = 
					CreateAst(lexia::Token::FUNCTION_CALL_TOKEN());
				func_call->AddChild(CreateAst(
					matcher(lexia::TokenType::IDENTIFIER())));
				matcher(lexia::TokenType::LEFT_PARENTHESIS());
				matcher(lexia::TokenType::RIGHT_PARENTHESIS());
				return func_call;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##postfix_expression2");
				auto func_call = CreateAst(lexia::Token::FUNCTION_CALL_TOKEN());
				func_call->AddChild(CreateAst(
					matcher(lexia::TokenType::IDENTIFIER())));
				matcher(lexia::TokenType::LEFT_PARENTHESIS());
				func_call->AddChild(processor("argument_expression_list"));
				matcher(lexia::TokenType::RIGHT_PARENTHESIS());
				return func_call;
			}))
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##postfix_expression3");
				return processor("primary_expression");	
			}));

		lang_parser_.DefineSyntaxRule("primary_expression")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##primary_expression");
				const auto next_token = looker(1);
				if(IsTokenTypeSame(next_token, 
						lexia::TokenType::IDENTIFIER())){	
					auto var_ref = 
						CreateAst(lexia::Token::VARIABLE_REFERENCE_TOKEN());
					var_ref->AddChild(
						CreateAst(matcher(lexia::TokenType::IDENTIFIER())));
					return var_ref;
				}
				else if(IsTokenTypeSame(next_token, 
						lexia::TokenType::CONSTANT())){	
					return CreateAst(
						matcher(lexia::TokenType::CONSTANT()));	
				}
				else {
					matcher(lexia::TokenType::LEFT_PARENTHESIS());	
					auto exp = processor("expression");
					matcher(lexia::TokenType::RIGHT_PARENTHESIS());	
					return exp;
				}
			}));

		lang_parser_.DefineSyntaxRule("argument_expression_list")
			->AddChoice(LangParser::SyntaxRule::Choice([this](
					PP_LANG_PARSER_SYNTAX_RULE_ARGUMENTS) -> const Ast::Ptr { 
				lang_parser_.DebugPrint("##argument_expression_list");
				auto param_list = CreateAst(lexia::Token::PARAMETER_LIST_TOKEN());
				auto parameter = CreateAst(lexia::Token::PARAMETER_TOKEN());
				param_list->AddChild(parameter);
				parameter->AddChild(processor("assign_expression"));
				while(IsTokenTypeSame(looker(1), lexia::TokenType::COMMA())){ 
					matcher(lexia::TokenType::COMMA());
					parameter = CreateAst(lexia::Token::PARAMETER_TOKEN());
					param_list->AddChild(parameter);
					parameter->AddChild(processor("assign_expression"));
				}	

				return param_list;
			}));
		
	}

};
}

