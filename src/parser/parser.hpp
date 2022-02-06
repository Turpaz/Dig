#ifndef PARSER_PARSER_HPP
#define PARSER_PARSER_HPP

#include "tree.hpp"
#include "../grammar/grammar.hpp"
#include "../lexer/lexer.hpp"

class Parser
{
private:
	Lexer* lexer;
	Nodes::StatementBlock program;
public:
	Parser(Lexer* lexer);

	void parse_token(const Token& tok);
	void print_tree();
private:
	void error(const Token& tok, const char* format, ...);

	void parse_statement(const Token& tok);
	void parse_expression(const Token& tok);

	// TODO: maybe add more parse_things
};

#endif // PARSER_PARSER_HPP