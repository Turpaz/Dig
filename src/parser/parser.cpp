#include "parser.hpp"

Parser::Parser(Lexer* lexer)
{
	this->lexer = lexer;
}

void Parser::parse_token(const Token& tok)
{
	switch (tok.type)
	{
	case toktype::IDENTIFIER:
		break;
	default:

	}
}

void Parser::print_tree()
{
	
}

void Parser::error(const Token& tok, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	this->lexer->error(tok.position, format, args);
	va_end(args);
}