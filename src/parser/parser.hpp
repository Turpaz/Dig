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
	void warning(const TokenNode& tok, const char* format, ...) const;

	Nodes::Statement* parse_statement(TokenNode& tok, int skip=0) const;
	Nodes::Statement* parse_keyword(TokenNode& tok, int skip=0) const;
	Nodes::Expression* parse_expression(TokenNode& tok, int skip=0) const;
	Nodes::StatementBlock* parse_block(TokenNode& tok, int skip=0) const;

	inline Nodes::Statement* parse_import(TokenNode& tok, int skip=0) const;
	inline Nodes::Statement* parse_return(TokenNode& tok, int skip=0) const;
	inline Nodes::Statement* parse_for(TokenNode& tok, int skip=0) const;
	inline Nodes::While* parse_while(TokenNode& tok, int skip=0) const;
	inline Nodes::Break* parse_break(TokenNode& tok, int skip=0) const;
	inline Nodes::Continue* parse_continue(TokenNode& tok, int skip=0) const;
	inline Nodes::Ite* parse_if(TokenNode& tok, int skip=0) const;
	// TODO: inline Nodes::ClassDecl* parse_class(TokenNode& tok, int skip=0) const;
	inline Nodes::NamespaceDecl* parse_namespace(TokenNode& tok, int skip=0) const;
	inline Nodes::FunctionDecl* parse_function(TokenNode& tok, int skip=0) const;
	inline Nodes::VarDecl* parse_variable(TokenNode& tok, int skip=0) const;

	inline Nodes::Statement* parse_expression_statement(TokenNode& tok, int skip=0) const;

	template<typename T>
	inline T* incRet(T* statement, TokenNode& tok, size_t times) const { for (size_t i = 0; i < times; i++) tok = *tok.next; return statement; };
};

#endif // PARSER_PARSER_HPP