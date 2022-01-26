#ifndef LEXER_TOKEN_HPP
#define LEXER_TOKEN_HPP

#include <string>
#include <cstdint>
#include <stdio.h>
#include "../grammar/grammar.hpp"

using std::string;

class Token
{
public:
	toktype type;
	string str; // Sometimes the value of the token
	union { long double num; unsigned keyword/* A keyword or an operator */; }; // Sometimes the value of the token
	size_t position;
public:
	Token(toktype type, string str, size_t position) : type(type), str(str), position(position) {}
	Token(toktype type, long double num, size_t position) : type(type), num(num), position(position) {}
	Token(toktype type, unsigned keyword, size_t position) : type(type), keyword(keyword), position(position) {}
	Token() : type(toktype::TOK_EOF), str(""), num(0), position(-1) {}
	Token(const Token& other) : type(other.type), str(other.str), num(other.num), position(other.position) {}

	void print()
	{
		if (type == toktype::IDENTIFIER || type == toktype::STRING)
			printf("(\"%s\" : %s) at %zu\n", str.c_str(), type == toktype::STRING ? "STRING" : "IDENTIFIER", position);
		else if (type == toktype::NUM)
			printf("(%Lf : %s) at %zu\n", num, "NUM", position);
		else if (type == toktype::KEYWORD)
			printf("(%s : %s) at %zu\n", getStringFromId(keyword).c_str(), "KEYWORD", position);
		else
			printf("(\"\" : %s) at %zu\n", getStrType(), position);
	}
private:
	inline const char* getStrType()
	{
		switch (type)
		{
			case toktype::IDENTIFIER:
				return "IDENTIFIER";
			case toktype::KEYWORD:
				return "KEYWORD";
			case toktype::NUM:
				return "NUM";
			case toktype::STRING:
				return "STRING";
			case toktype::TOK_EOF:
				return "EOF";
			default:
				return "ERROR_TYPE";
		}
	}
};

#endif // LEXER_TOKEN_HPP