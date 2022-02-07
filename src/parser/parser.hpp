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

	void parse(const TokenNode& tok);
	void print();
private:
	void error(const TokenNode& tok, const char* format, ...);

	Nodes::Statement* parse_statement(const TokenNode& tok);
	Nodes::Statement* parse_keyword(const TokenNode& tok);
	Nodes::Expression* parse_expression(const TokenNode& tok);
	Nodes::StatementBlock* parse_block(const TokenNode& tok);

	// TODO: maybe add more parse_things
};

#endif // PARSER_PARSER_HPP