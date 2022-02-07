#include "parser.hpp"

Parser::Parser(Lexer* lexer)
{
	this->lexer = lexer;
}

void Parser::parse(const TokenNode& tok)
{
	this->program.statements.push_back(parse_statement(tok));
}

void Parser::print()
{
	for (auto& statement : this->program.statements)
	{
		statement->print();
	}
}

Nodes::Statement* Parser::parse_statement(const TokenNode& tok)
{
	switch(tok.tok.type)
	{
		case toktype::TOK_EOF:
			break; // TODO: maybe do something more, such as stop parsing
		case toktype::KEYWORD:
			return parse_keyword(tok);
		case toktype::IDENTIFIER: case toktype::STRING: case toktype::NUM: case toktype::OPERATOR:
			if (tok.tok.keyword == uenum(operators::LBRACE))
				return parse_block(tok);
			else return new Nodes::ExpressionStatement{tok.tok.position, parse_expression(tok)};
		case toktype::ROOT:
			//return new Nodes::RootStatement{tok.tok.position};
			return parse_statement(*tok.next);
		default:
			error(tok, "Unexpected token: %s", tok.tok.str.c_str());
	}

	return new Nodes::Statement{tok.tok.position};
}

Nodes::Statement* Parser::parse_keyword(const TokenNode& tok)
{
	switch (tok.tok.keyword)
	{
	case uenum(keywords::IMPORT): 			// -----=====*****\ IMPORT /*****=====-----
		// import "..."
		if (tok.next->tok.type == toktype::STRING)
		{
			// import "...";
			if (tok.next->next->tok.type == toktype::OPERATOR && tok.next->next->tok.keyword == uenum(operators::SEMICOLON))
				return new Nodes::ImportFile{tok.tok.position, tok.next->tok.str, GET_JUST_FILENAME(tok.next->tok.str)};
			 // import "..." as ...;
			else if (tok.next->next->tok.type == toktype::KEYWORD && tok.next->next->tok.keyword == uenum(keywords::AS))
			{
				// Make sure we have a semicolon after the import
				if (tok.next->next->next->next->tok.type == toktype::OPERATOR && tok.next->next->next->next->tok.keyword == uenum(operators::SEMICOLON))
					;
				else error(tok, "Expected ';' after 'import \"...\" as ...' statement (import \"...\" as ...;)");
				// Make sure we have an identifier after the as and return the import statement
				if (tok.next->next->next->tok.type == toktype::IDENTIFIER)
					return new Nodes::ImportFile{tok.tok.position, tok.next->tok.str, tok.next->next->next->tok.str};
				else error(tok, "Expected identifier after 'import \"...\" as' statement (import \"...\" as ...;)");
			}
			// Throw an error if we don't have a semicolon after the import
			else error(tok, "Expected ';' after 'import \"...\"' statement (import \"...\";)");
		}
		// import ...
		else if (tok.next->tok.type == toktype::IDENTIFIER)
		{
			// import ...;
			if (tok.next->next->tok.type == toktype::OPERATOR && tok.next->next->tok.keyword == uenum(operators::SEMICOLON))
				return new Nodes::ImportModule{tok.tok.position, tok.next->tok.str, tok.next->tok.str};
			// import ... as ...;
			else if (tok.next->next->tok.type == toktype::KEYWORD && tok.next->next->tok.keyword == uenum(keywords::AS))
			{
				// Make sure we have a semicolon after the import
				if (tok.next->next->next->next->tok.type == toktype::OPERATOR && tok.next->next->next->next->tok.keyword == uenum(operators::SEMICOLON))
					;
				else error(tok, "Expected ';' after 'import ... as ...' statement (import ... as ...;)");
				// Make sure we have an identifier after the as and return the import statement
				if (tok.next->next->next->tok.type == toktype::IDENTIFIER)
					return new Nodes::ImportModule{tok.tok.position, tok.next->tok.str, tok.next->next->next->tok.str};
				else error(tok, "Expected identifier after 'import ... as' statement (import ... as ...;)");
			}
			// Throw an error if we don't have a semicolon after the import
			else error(tok, "Expected ';' after 'import ...' statement (import ...;)");
		}
		// Throw an error if we don't have an identifier or string after the import
		else error(tok, "Expected identifier or string after 'import' statement (import ...; / import \"...\";)");
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

	return new Nodes::Statement{tok.tok.position};
}



void Parser::error(const TokenNode& tok, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	this->lexer->error(tok.tok.position, format, args);
	va_end(args);
}

Nodes::Expression* Parser::parse_expression(const TokenNode& tok)
{
	return new Nodes::Expression{tok.tok.position};
}

Nodes::StatementBlock* Parser::parse_block(const TokenNode& tok)
{
	return new Nodes::StatementBlock{tok.tok.position, vector<Nodes::Statement*>()};
}