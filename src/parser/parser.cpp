#include "parser.hpp"

Parser::Parser(Lexer* lexer)
{
	this->lexer = lexer;
}

void Parser::parse_token(const Token& tok)
{
	switch(tok.type)
	{
		case toktype::TOK_EOF:
			break;
		case toktype::IDENTIFIER:
			parse_statement(tok);
			// We might call a function so it might be an expression
			break;
		case toktype::KEYWORD:
			parse_statement(tok);
			break;
		case toktype::STRING:
			parse_expression(tok);
			break;
		case toktype::NUM:
			parse_expression(tok);
			break;
		default:
			error(tok, "Unexpected token: %s", tok.str.c_str());
	}
}

void Parser::print_tree()
{
	for (auto &&i : this->program.statements)
	{
		//i->print(); // TODO: Implement this
	}
}

void Parser::error(const Token& tok, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	this->lexer->error(tok.position, format, args);
	va_end(args);
}