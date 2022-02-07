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
	void print() const;
private:
	void error(const TokenNode& tok, const char* format, ...) const;

	Nodes::Statement* parse_statement(TokenNode& tok, int skip=0) const;
	Nodes::Statement* parse_keyword(TokenNode& tok, int skip=0) const;
	Nodes::Expression* parse_expression(TokenNode& tok, int skip=0) const;
	Nodes::StatementBlock* parse_block(TokenNode& tok, int skip=0) const;

	inline Nodes::Statement* parse_import(TokenNode& tok, int skip=0) const;
	inline Nodes::Statement* parse_expression_statement(TokenNode& tok, int skip=0) const;

	inline Nodes::Statement* incRet(Nodes::Statement* statement, TokenNode& tok, size_t times) const { for (size_t i = 0; i < times; i++) tok = *tok.next; return statement; };
	inline Nodes::Expression* incRet(Nodes::Expression* expression, TokenNode& tok, size_t times) const { for (size_t i = 0; i < times; i++) tok = *tok.next; return expression; };
};

#endif // PARSER_PARSER_HPP