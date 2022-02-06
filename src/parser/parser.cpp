#include "parser.hpp"

Parser::Parser(Lexer* lexer)
{
	this->lexer = lexer;
}

void Parser::parse(const TokenNode& tok)
{
	parse_statement(tok);
}

void Parser::print_tree()
{
	for (auto &&i : this->program.statements)
	{
		//i->print(); // TODO: Implement this
	}
}

Nodes::Statement Parser::parse_statement(const TokenNode& tok)
{
	switch(tok.tok.type)
	{
		case toktype::TOK_EOF:
			break; // TODO: maybe do something more, such as stop parsing
		case toktype::KEYWORD:
			return parse_keyword(tok);
		case toktype::IDENTIFIER: case toktype::STRING: case toktype::NUM: case toktype::OPERATOR:
			if (tok.tok.keyword == uenum(operators::LBRACE))
			{
				return parse_block(tok);
				break;
			} else return Nodes::ExpressionStatement{tok.tok.position, parse_expression(tok)};
		default:
			error(tok, "Unexpected token: %s", tok.tok.str.c_str());
	}
}

Nodes::Statement Parser::parse_keyword(const TokenNode& tok)
{
	switch (tok.tok.keyword)
	{
	case uenum(keywords::IMPORT):
		break;
	case uenum(keywords::RETURN):
		break;
	case uenum(keywords::FOR):
		break;
	case uenum(keywords::WHILE):
		break;
	case uenum(keywords::BREAK):
		break;
	case uenum(keywords::CONTINUE):
		break;
	case uenum(keywords::IF):
		break;
	case uenum(keywords::CLASS):
		break;
	case uenum(keywords::NAMESPACE):
		break;
	case uenum(keywords::FUN):
		break;
	// VARTYPES
	case uenum(vartypes::INT):
		break;
	case uenum(vartypes::FLOAT):
		break;
	case uenum(vartypes::STR):
		break;
	case uenum(vartypes::BOOL):
		break;
	case uenum(vartypes::ARR):
		break;
	case uenum(vartypes::VAR):
		break;
	default:
		error(tok, "Unexpected keyword: %s", tok.tok.str.c_str());
	}
}



void Parser::error(const TokenNode& tok, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	this->lexer->error(tok.tok.position, format, args);
	va_end(args);
}