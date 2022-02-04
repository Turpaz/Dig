#ifndef PARSER_PARSER_HPP
#define PARSER_PARSER_HPP

#include "tree.hpp"
#include "../lexer/lexer.hpp"

class Parser
{
private:
	Lexer* lexer;
	StatementBlock program;
public:
	Parser(Lexer* lexer);

	void parse_token(const Token& tok);
	void print_tree();
private:
	void error(const Token& tok, const char* format, ...);
};

#endif // PARSER_PARSER_HPP