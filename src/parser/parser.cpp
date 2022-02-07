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
			
			else return parse_expression_statement(tok);
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
		return parse_import(tok, 1);
	case uenum(keywords::RETURN): 			// -----=====*****\ RETURN /*****=====-----
		return parse_return(tok, 1);
	case uenum(keywords::FOR):				// -----=====*****\ FOR /*****=====-----
		return parse_for(tok, 1);
	case uenum(keywords::WHILE): 			// -----=====*****\ WHILE /*****=====-----
		return parse_while(tok, 1);
	case uenum(keywords::BREAK): 			// -----=====*****\ BREAK /*****=====-----
		return parse_break(tok, 1);
	case uenum(keywords::CONTINUE): 		// -----=====*****\ CONTINUE /*****=====-----
		return parse_continue(tok, 1);
	case uenum(keywords::IF): 				// -----=====*****\ IF /*****=====-----
		return parse_if(tok, 1);
	case uenum(keywords::CLASS): 			// -----=====*****\ CLASS /*****=====-----
		// TODO: return parse_class(tok, 1);
	case uenum(keywords::NAMESPACE): 		// -----=====*****\ NAMESPACE /*****=====-----
		return parse_namespace(tok, 1);
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
	else error(tok, "Expected '{ ... }'");
	// TODO: Add support for single statement blocks e.g. if (true) print("hello"); else print("world");
	// or
	// if (true)
	//     print("hello");

	// no need to incRet beacause we're incrementing in the loop
	return block;
}

inline Nodes::Statement* Parser::parse_import(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	// import "..."
	if (tok.tok.type == toktype::STRING)
	{
		// import "...";
		if (tok.next->tok.type == toktype::OPERATOR && tok.next->tok.keyword == uenum(operators::SEMICOLON))
		{
			return incRet(
				new Nodes::ImportFile{pos, tok.tok.str, GET_JUST_FILENAME(tok.tok.str)},
				tok, 3);
		}
			// import "..." as ...;
		else if (tok.next->tok.type == toktype::KEYWORD && tok.next->tok.keyword == uenum(keywords::AS))
		{
			// Make sure we have a semicolon after the import
			if (tok.next->next->next->tok.type == toktype::OPERATOR && tok.next->next->next->tok.keyword == uenum(operators::SEMICOLON))
				;
			else error(tok, "Expected ';' after 'import \"...\" as ...' statement (import \"...\" as ...;)");
			// Make sure we have an identifier after the as and return the import statement
			if (tok.next->next->tok.type == toktype::IDENTIFIER)
			{
				return incRet(
					new Nodes::ImportFile{pos, tok.tok.str, tok.next->next->tok.str},
					tok, 5);
			}
			else error(tok, "Expected identifier after 'import \"...\" as' statement (import \"...\" as ...;)");
		}
		// Throw an error if we don't have a semicolon after the import
		else error(tok, "Expected ';' after 'import \"...\"' statement (import \"...\";)");
	}
	// import ...
	else if (tok.tok.type == toktype::IDENTIFIER)
	{
		// import ...;
		if (tok.next->tok.type == toktype::OPERATOR && tok.next->tok.keyword == uenum(operators::SEMICOLON))
		{
			return incRet(
				new Nodes::ImportModule{pos, tok.tok.str, tok.tok.str},
				tok, 3);
		}
		// import ... as ...;
		else if (tok.next->tok.type == toktype::KEYWORD && tok.next->tok.keyword == uenum(keywords::AS))
		{
			// Make sure we have a semicolon after the import
			if (tok.next->next->next->tok.type == toktype::OPERATOR && tok.next->next->next->tok.keyword == uenum(operators::SEMICOLON))
				;
			else error(tok, "Expected ';' after 'import ... as ...' statement (import ... as ...;)");
			// Make sure we have an identifier after the as and return the import statement
			if (tok.next->next->tok.type == toktype::IDENTIFIER)
			{
				return incRet(
					new Nodes::ImportModule{pos, tok.tok.str, tok.next->next->tok.str},
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
		new Nodes::Statement{pos},
		tok, 1);
}
inline Nodes::Statement* Parser::parse_return(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::SEMICOLON))
		return incRet(
			new Nodes::Return{pos, new Nodes::NullLiteralExpression{pos}},
			tok, 2);
	else if (tok.next->tok.type == toktype::OPERATOR && tok.next->tok.keyword == uenum(operators::SEMICOLON))
		return incRet(
			new Nodes::Return{pos, parse_expression(tok, 1)},
			tok, 1); // skip one in parse_expression and the semicolon afterwards
	else error(tok, "Expected semicolon after return statement (return <expression>; or return;)");

	// We should never reach this point
	return incRet(
		new Nodes::Statement{pos},
		tok, 1);
}
inline Nodes::Statement* Parser::parse_for(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	Nodes::Expression* init;
	Nodes::Expression* cond;
	Nodes::Expression* inc;
	Nodes::Expression* iterOrNum;
	Nodes::StatementBlock* body;
	
	// Get the init statement
	init = parse_expression(*tok.next);
	// for init; cond; inc body
	if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::SEMICOLON))
	{
		// Get the condition skip the semicolon
		cond = parse_expression(tok, 1);
		// for init; cond; inc body
		if (tok.next->tok.type == toktype::OPERATOR && tok.next->tok.keyword == uenum(operators::SEMICOLON))
		{
			// Get the increment (Skip the semicolon)
			inc = parse_expression(tok, 1);
			// Get the body
			body = parse_block(tok);
			return incRet(
				new Nodes::For{pos, init, cond, inc, body},
				tok, 0);
		}
		else error(tok, "Expected ';' in for statement (for <init>; <cond>; <inc> <body>)");
	}
	// for init : INT body || for init : ITER body
	else if (tok.next->tok.type == toktype::OPERATOR && tok.next->tok.keyword == uenum(operators::COLON))
	{
		// Get the ITER or the INT skip the colon
		iterOrNum = parse_expression(tok, 1);
		// Get the body
		body = parse_block(tok);
		return incRet(
			new Nodes::ForIter{pos, init, iterOrNum, body},
			tok, 0);
	}
	else error(tok, "Expected ';' or ':' in for statement (for <init>; <cond>; <inc> <body>)");

	// We should never reach this point
	return incRet(
		new Nodes::Statement{pos},
		tok, 1);
}
inline Nodes::While* Parser::parse_while(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	// parse the condition
	auto condition = parse_expression(tok);
	// parse the block
	auto block = parse_block(tok);
	// return the while statement
	return incRet(
		new Nodes::While{pos, condition, block},
		tok, 0);
}
inline Nodes::Break* Parser::parse_break(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	if (tok.tok.keyword == uenum(operators::SEMICOLON))
		return incRet(
			new Nodes::Break{pos},
			tok, 2);
	else error(tok, "Expected ';' after 'break' (break;)");
}
inline Nodes::Continue* Parser::parse_continue(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	if (tok.tok.keyword == uenum(operators::SEMICOLON))
		return incRet(
			new Nodes::Continue{pos},
			tok, 2);
	else error(tok, "Expected ';' after 'continue' (continue;)");
}
inline Nodes::Ite* Parser::parse_if(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	// parse the condition
	auto condition = parse_expression(tok);
	// parse the block
	auto block = parse_block(tok);
	
	if (tok.tok.type == toktype::KEYWORD && tok.tok.keyword == uenum(keywords::ELSE))
	{
		// parse the else block
		auto elseBlock = parse_block(tok, 1);

		return incRet(
			new Nodes::Ite{pos, condition, *block, *elseBlock},
			tok, 0);
	}
	else if (tok.tok.type == toktype::KEYWORD && tok.tok.keyword == uenum(keywords::ELIF))
	{
		size_t elif_pos = tok.tok.position;
		// parse the else if block
		auto elseIf = parse_if(tok, 1);

		return incRet(
			new Nodes::Ite{pos, condition, *block, Nodes::StatementBlock{elif_pos, {elseIf}}},
			tok, 0);
	}
	else return incRet(
		new Nodes::Ite{pos, condition, *block, Nodes::StatementBlock{tok.tok.position}},
		tok, 0);

	// We should never reach this point
	return incRet(
		new Nodes::Ite{pos, condition, Nodes::StatementBlock{}, Nodes::StatementBlock{}},
		tok, 1);
}
inline Nodes::NamespaceDecl* Parser::parse_namespace(TokenNode& tok, int skip=0) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	if (tok.tok.type == toktype::IDENTIFIER)
	{
		// Get the namespace name
		auto name = tok.tok.str;
		// Get the namespace block
		auto block = parse_block(tok);
		// Return the namespace
		return incRet(
			new Nodes::NamespaceDecl{pos, name, *block},
			tok, 0);
	}
	else error(tok, "Expected identifier namespace name (namespace <name> { <block> })");

	// We should never reach this point
	return incRet(
		new Nodes::NamespaceDecl{pos, "", Nodes::StatementBlock{}},
		tok, 1);
}

inline Nodes::Statement* Parser::parse_expression_statement(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	Nodes::Statement* expr = new Nodes::ExpressionStatement{pos, parse_expression(tok, 1)};

	if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::SEMICOLON))
	{
		return incRet(expr, tok, 1);
	}
	// Throw an error if we don't have a semicolon after the expression
	else error(tok, "Expected ';' after expression statement (expression;)");

	// We should never reach this point
	return incRet(
		new Nodes::Statement{pos},
		tok, 1);
}

void Parser::error(const TokenNode& tok, const char* format, ...) const
{
	va_list args;
	va_start(args, format);
	this->lexer->error(tok.tok.position, format, args);
	va_end(args);
}

void Parser::warning(const TokenNode& tok, const char* format, ...) const
{
	va_list args;
	va_start(args, format);
	this->lexer->warning(tok.tok.position, format, args);
	va_end(args);
}