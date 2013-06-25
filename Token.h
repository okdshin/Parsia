#pragma once
//Token:20130625
#include <iostream>

namespace parsia
{
class TokenType{
public:
	TokenType() : token_type_str(){}
	TokenType(const std::string& token_type_str) : token_type_str(token_type_str){}

	static auto EOF_TOKEN_TYPE() -> TokenType {
		return TokenType("EOF_TOKEN_TYPE");	
	}

	static auto UNINITIALIZED_TOKEN_TYPE() -> TokenType {
		return TokenType("UNINITIALIZED_TOKEN_TYPE");	
	}

	auto ToString()const -> std::string {
		return token_type_str;	
	}

	auto operator==(const TokenType& pair)const -> bool { 
		return this->token_type_str == pair.token_type_str; 
	}

	auto operator!=(const TokenType& pair)const -> bool { 
		return !(*this == pair); 
	}

private:
	std::string token_type_str;
};
auto operator<<(std::ostream& os, const TokenType& token_type) -> std::ostream& {
	os << "TokenType:" << token_type.ToString();	
	return os;	
}

class Word{
public:
	Word() : word_str_(){}
    Word(const std::string& word_str) : word_str_(word_str){}

	auto ToString()const -> std::string {
		return word_str_;	
	}

private:
	std::string word_str_;
};
auto operator<<(std::ostream& os, const Word& word) -> std::ostream& {
	os << "Word:" << "\"" << word.ToString() << "\"";
	return os;
}

class Token{
public:
	Token():type_(), word_(){}
	Token(const TokenType& type, const Word& word) 
		: type_(type), word_(word){}
	
	auto IsEof()const -> bool {
		return type_ == TokenType::EOF_TOKEN_TYPE();		
	}
	
	auto IsUninitialized()const -> bool {
		return type_ == TokenType::UNINITIALIZED_TOKEN_TYPE();		
	}

	auto GetType()const -> TokenType {
		return type_;
	}

	auto GetWord()const -> Word {
		return word_;
	}
	
private:
	TokenType type_;
	Word word_;
};

auto operator<<(std::ostream& os, const Token& token) -> std::ostream& {
	os << "Token:" << token.GetType() << ", " << token.GetWord();
	return os;	
}

auto operator==(const Token& left, const Token& right) -> bool {
	return left.GetType() == right.GetType();
}
}

