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
	union { double num; unsigned keyword/* A keyword, operator, vartype */; }; // Sometimes the value of the token
	size_t position;
public:
	Token(toktype type, string str, size_t position) : type(type), str(str), position(position) {}
	Token(toktype type, double num, size_t position) : type(type), num(num), position(position) {}
	Token(toktype type, unsigned keyword, size_t position) : type(type), keyword(keyword), position(position) {}
	Token() : type(toktype::TOK_EOF), str(""), num(0), position(-1) {}
	Token(const Token& other) : type(other.type), str(other.str), num(other.num), position(other.position) {}

	void print()
	{
		if (type == toktype::IDENTIFIER || type == toktype::STRING)
			printf("(\"%s\" : %s) at %zu\n", str.c_str(), type == toktype::STRING ? "STRING" : "IDENTIFIER", position);
		else if (type == toktype::NUM)
			printf("(%f : NUM) at %zu\n", num, position);
		else if (type == toktype::KEYWORD)
			printf("(%s : KEYWORD) at %zu\n", getStringFromId(keyword).c_str(), position);
		else if (type == toktype::OPERATOR)
			printf("(%s : OPERATOR) at %zu\n", getStringFromId(keyword).c_str(), position);
		else
			printf("(\"\" : %s) at %zu\n", type == toktype::TOK_EOF ? "EOF" : "ERROR_TYPE", position);
	}
private:
};

#endif // LEXER_TOKEN_HPP