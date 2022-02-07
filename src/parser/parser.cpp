#include "parser.hpp"

Parser::Parser(Lexer* lexer)
{
	this->lexer = lexer;
}

void Parser::parse(TokenNode& root)
{
	// Parse until we reach the end of the file
	while ( root.tok.type != toktype::TOK_EOF )
	{
		// Will also increment the token
		this->program.statements.push_back(parse_statement(root));
		
		this->program.statements[this->program.statements.size() - 1]->print(); // Temporary
	}
}

void Parser::print() const
{
	for (auto& statement : this->program.statements)
	{
		statement->print();
	}
}

Nodes::Statement* Parser::parse_statement(TokenNode& tok, int skip) const
{
	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	switch(tok.tok.type)
	{
		case toktype::TOK_EOF:
			break; // TODO: maybe do something more, such as stop parsing
		case toktype::KEYWORD:
			return parse_keyword(tok);
		case toktype::IDENTIFIER: case toktype::STRING: case toktype::NUM: case toktype::OPERATOR:
			if (tok.tok.keyword == uenum(operators::LBRACE))
				return parse_block(tok);
			if (tok.tok.keyword == uenum(operators::SEMICOLON))
				return incRet(
					new Nodes::EmptyStatement{tok.tok.position},
					tok, 1);
			
			else return new Nodes::ExpressionStatement{tok.tok.position, parse_expression(tok)}; // no incRet because we already incremented in parse_expression
		case toktype::ROOT:
			return incRet(
				new Nodes::RootStatement{tok.tok.position},
				tok, 1);
		default:
			error(tok, "Unexpected token: %s", tok.tok.str.c_str());
	}

	return incRet(
		new Nodes::Statement{tok.tok.position},
		tok, 1);
}

Nodes::Statement* Parser::parse_keyword(TokenNode& tok, int skip) const
{
	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	switch (tok.tok.keyword)
	{
	case uenum(keywords::IMPORT): 			// -----=====*****\ IMPORT /*****=====-----
		return parse_import(tok);
	case uenum(keywords::RETURN): 			// -----=====*****\ RETURN /*****=====-----
		if (tok.next->tok.type == toktype::OPERATOR && tok.next->tok.keyword == uenum(operators::SEMICOLON))
			return incRet(
				new Nodes::Return{tok.tok.position, new Nodes::NullLiteralExpression{tok.tok.position}},
				tok, 2);
		else if (tok.next->next->tok.type == toktype::OPERATOR && tok.next->next->tok.keyword == uenum(operators::SEMICOLON))
			return incRet(
				new Nodes::Return{tok.tok.position, parse_expression(tok, 1)},
				tok, 1); // skip one in parse_expression and the semicolon afterwards
		else error(tok, "Expected semicolon after return statement (return <expression>; or return;)");
	case uenum(keywords::FOR):				// -----=====*****\ FOR /*****=====-----
		Nodes::Statement* init;
		Nodes::Expression* cond;
		Nodes::Expression* inc;
		Nodes::Statement* body;
		
		// for init; cond; inc; body || for init : INT body || for init : ITER body
		init = parse_statement(*tok.next); // Get the init statement

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

	return incRet(
		new Nodes::Statement{tok.tok.position},
		tok, 1);
}

Nodes::Expression* Parser::parse_expression(TokenNode& tok, int skip) const
{
	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	// TODO: Implement

	return incRet(
		new Nodes::Expression{tok.tok.position},
		tok, 1);
}

Nodes::StatementBlock* Parser::parse_block(TokenNode& tok, int skip) const
{
	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	Nodes::StatementBlock* block = new Nodes::StatementBlock{tok.tok.position, vector<Nodes::Statement*>()};

	// Make sure we have a '{'
	if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::LBRACE))
	{
		while ( !(tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::RBRACE)) )
		{
			// if we reached the end of the file, throw an error
			if (tok.tok.type == toktype::TOK_EOF)
				error(tok, "Expected '}' to end the block at %zu", block->position);
			
			// push the statement onto the block, will increment the token too
			block->statements.push_back(parse_statement(tok));
		}

		// skip the '}'
		tok = *tok.next;
	}

	// no need to incRet beacause we're incrementing in the loop
	return block;
}

inline Nodes::Statement* Parser::parse_import(TokenNode& tok, int skip) const
{
	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	// import "..."
	if (tok.next->tok.type == toktype::STRING)
	{
		// import "...";
		if (tok.next->next->tok.type == toktype::OPERATOR && tok.next->next->tok.keyword == uenum(operators::SEMICOLON))
		{
			return incRet(
				new Nodes::ImportFile{tok.tok.position, tok.next->tok.str, GET_JUST_FILENAME(tok.next->tok.str)},
				tok, 3);
		}
			// import "..." as ...;
		else if (tok.next->next->tok.type == toktype::KEYWORD && tok.next->next->tok.keyword == uenum(keywords::AS))
		{
			// Make sure we have a semicolon after the import
			if (tok.next->next->next->next->tok.type == toktype::OPERATOR && tok.next->next->next->next->tok.keyword == uenum(operators::SEMICOLON))
				;
			else error(tok, "Expected ';' after 'import \"...\" as ...' statement (import \"...\" as ...;)");
			// Make sure we have an identifier after the as and return the import statement
			if (tok.next->next->next->tok.type == toktype::IDENTIFIER)
			{
				return incRet(
					new Nodes::ImportFile{tok.tok.position, tok.next->tok.str, tok.next->next->next->tok.str},
					tok, 5);
			}
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
		{
			return incRet(
				new Nodes::ImportModule{tok.tok.position, tok.next->tok.str, tok.next->tok.str},
				tok, 3);
		}
		// import ... as ...;
		else if (tok.next->next->tok.type == toktype::KEYWORD && tok.next->next->tok.keyword == uenum(keywords::AS))
		{
			// Make sure we have a semicolon after the import
			if (tok.next->next->next->next->tok.type == toktype::OPERATOR && tok.next->next->next->next->tok.keyword == uenum(operators::SEMICOLON))
				;
			else error(tok, "Expected ';' after 'import ... as ...' statement (import ... as ...;)");
			// Make sure we have an identifier after the as and return the import statement
			if (tok.next->next->next->tok.type == toktype::IDENTIFIER)
			{
				return incRet(
					new Nodes::ImportModule{tok.tok.position, tok.next->tok.str, tok.next->next->next->tok.str},
					tok, 5);
			}
			else error(tok, "Expected identifier after 'import ... as' statement (import ... as ...;)");
		}
		// Throw an error if we don't have a semicolon after the import
		else error(tok, "Expected ';' after 'import ...' statement (import ...;)");
	}
	// Throw an error if we don't have an identifier or string after the import
	else error(tok, "Expected identifier or string after 'import' statement (import ...; / import \"...\";)");

	// We should never reach this point
	return incRet(
		new Nodes::Statement{tok.tok.position},
		tok, 1);
}

inline Nodes::Statement* Parser::parse_expression_statement(TokenNode& tok, int skip) const
{
	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	// expression;
	if (tok.next->tok.type == toktype::OPERATOR && tok.next->tok.keyword == uenum(operators::SEMICOLON))
	{
		return incRet(
			new Nodes::ExpressionStatement{tok.tok.position, parse_expression(tok)},
			tok, 2);
	}
	// Throw an error if we don't have a semicolon after the expression
	else error(tok, "Expected ';' after expression statement (expression;)");

	// We should never reach this point
	return incRet(
		new Nodes::Statement{tok.tok.position},
		tok, 1);
}

void Parser::error(const TokenNode& tok, const char* format, ...) const
{
	va_list args;
	va_start(args, format);
	this->lexer->error(tok.tok.position, format, args);
	va_end(args);
}