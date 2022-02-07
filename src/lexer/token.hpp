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
	Token(toktype type, size_t position) : type(type), position(position) {}
	Token(size_t position) : type(toktype::TOK_EOF), position(position) {}
	Token() : type(toktype::TOK_EOF), str(""), num(0), position(-1) {}

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
		else if (type == toktype::TOK_EOF)
			printf("(EOF : EOF) at %zu\n", position);
		else if (type == toktype::ROOT)
			printf("(ROOT : ROOT) at %zu\n", position);
		else
			printf("(\"\" : ERROR_TYPE) at %zu\n", position);
	}
private:
};

struct TokenNode
{
	Token tok;
	TokenNode* next;
	TokenNode(Token token, TokenNode* next=NULL) : tok(token), next(next) {}
	TokenNode() : tok(toktype::ROOT, 0u, 0), next(NULL) {}

	void add_child(Token child)
	{
		if (next == NULL)
			next = new TokenNode(child);
		else
			next->add_child(child);
	}

	void print()
	{
		printf("TokenNode: ");
		tok.print();
		if (next != NULL)
			next->print();
	}
};

#endif // LEXER_TOKEN_HPP