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
	case uenum(keywords::IMPORT): 			// -----=====*****\ IMPORT /*****=====-----
		// import "..."
		if (tok.next->tok.type == toktype::STRING)
		{
			// import "...";
			if (tok.next->next->tok.type == toktype::OPERATOR && tok.next->next->tok.keyword == uenum(operators::SEMICOLON))
				return Nodes::ImportFile{tok.tok.position, tok.next->tok.str, GET_JUST_FILENAME(tok.next->tok.str)};
			 // import "..." as ...;
			else if (tok.next->next->tok.type == toktype::KEYWORD && tok.next->next->tok.keyword == uenum(keywords::AS))
			{
				// Make sure we have a semicolon after the import
				if (tok.next->next->next->next->tok.type == toktype::KEYWORD && tok.next->next->next->next->tok.keyword == uenum(operators::SEMICOLON))
					;
				else error(tok, "Expected ';' after 'import \"...\" as ...' statement (import \"...\" as ...;)");
				// Make sure we have an identifier after the as and return the import statement
				if (tok.next->next->next->tok.type == toktype::IDENTIFIER)
					return Nodes::ImportFile{tok.tok.position, tok.next->tok.str, tok.next->next->next->tok.str};
				else error(tok, "Expected identifier after 'import \"...\" as' statement (import \"...\" as ...;)");
			}
			// Throw an error if we don't have a semicolon after the import
			else error(tok, "Expected ';' after 'import \"...\"' statement (import \"...\";)");
		}
		// import ...
		else if (tok.next->tok.type == toktype::IDENTIFIER)
		{
			// import ...;
			if (tok.next->next->tok.type == toktype::KEYWORD && tok.next->next->tok.keyword == uenum(operators::SEMICOLON))
				return Nodes::ImportModule{tok.tok.position, tok.next->tok.str};
			// import ... as ...;
			else if (tok.next->next->tok.type == toktype::KEYWORD && tok.next->next->tok.keyword == uenum(keywords::AS))
			{
				// Make sure we have a semicolon after the import
				if (tok.next->next->next->next->tok.type == toktype::KEYWORD && tok.next->next->next->next->tok.keyword == uenum(operators::SEMICOLON))
					;
				else error(tok, "Expected ';' after 'import ... as ...' statement (import ... as ...;)");
				// Make sure we have an identifier after the as and return the import statement
				if (tok.next->next->next->tok.type == toktype::IDENTIFIER)
					return Nodes::ImportModule{tok.tok.position, tok.next->tok.str, tok.next->next->next->tok.str};
				else error(tok, "Expected identifier after 'import ... as' statement (import ... as ...;)");
			}
			else error(tok, "Expected ';' after 'import ...' statement (import ...;)");
		}
		else error(tok, "Expected ';' after 'import' statement (import ...;)");
	case uenum(keywords::RETURN): 			// -----=====*****\ RETURN /*****=====-----
		break;
	case uenum(keywords::FOR):				// -----=====*****\ FOR /*****=====-----
		break;
	case uenum(keywords::WHILE): 			// -----=====*****\ WHILE /*****=====-----
		break;
	case uenum(keywords::BREAK): 			// -----=====*****\ BREAK /*****=====-----
		break;
	case uenum(keywords::CONTINUE): 		// -----=====*****\ CONTINUE /*****=====-----
		break;
	case uenum(keywords::IF): 				// -----=====*****\ IF /*****=====-----
		break;
	case uenum(keywords::CLASS): 			// -----=====*****\ CLASS /*****=====-----
		break;
	case uenum(keywords::NAMESPACE): 		// -----=====*****\ NAMESPACE /*****=====-----
		break;
	case uenum(keywords::FUN): 				// -----=====*****\ FUN /*****=====-----
		break;
	// VARTYPES
	case uenum(vartypes::INT): 				// -----=====*****\ INT /*****=====-----
		break;
	case uenum(vartypes::FLOAT): 			// -----=====*****\ FLOAT /*****=====-----
		break;
	case uenum(vartypes::STR): 				// -----=====*****\ STR /*****=====-----
		break;
	case uenum(vartypes::BOOL): 			// -----=====*****\ BOOL /*****=====-----
		break;
	case uenum(vartypes::ARR): 				// -----=====*****\ ARR /*****=====-----
		break;
	case uenum(vartypes::VAR): 				// -----=====*****\ VAR /*****=====-----
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