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

	void parse(TokenNode& root);
	void print();
private:
	void error(const TokenNode& tok, const char* format, ...) const;

	Nodes::Statement* parse_statement(TokenNode& tok);
	Nodes::Statement* parse_keyword(TokenNode& tok);
	Nodes::Expression* parse_expression(TokenNode& tok);
	Nodes::StatementBlock* parse_block(TokenNode& tok);

	inline Nodes::Statement* parse_import(TokenNode& tok) const;

	inline Nodes::Statement* incRet(Nodes::Statement* statement, TokenNode& tok, size_t times) const { for (size_t i = 0; i < times; i++) tok = *tok.next; return statement; };
	inline Nodes::Expression* incRet(Nodes::Expression* expression, TokenNode& tok, size_t times) const { for (size_t i = 0; i < times; i++) tok = *tok.next; return expression; };
};

#endif // PARSER_PARSER_HPP