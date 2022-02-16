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
		
		this->program.statements[this->program.statements.size() - 1]->print(); // TODO: remove (here because we're yet to actually compile)
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
			error(tok, "Unexpected end of file");
		case toktype::KEYWORD:
			return parse_keyword(tok);
		case toktype::IDENTIFIER: case toktype::STRING: case toktype::NUM: case toktype::OPERATOR:
			if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::LBRACE))
				return parse_block(tok);
			if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::SEMICOLON))
				return incRet(
					new Nodes::EmptyStatement{tok.tok.position},
					tok, 1);
			
			else return parse_expression_statement(tok);
		case toktype::ROOT:
			return incRet(
				new Nodes::RootStatement{tok.tok.position},
				tok, 1);
		default:
			error(tok, "Unexpected token: %s", tok.tok.tostr().c_str());
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

	size_t pos = tok.tok.position;

	// We'll need to keep track of the last expression we parsed
	Nodes::Expression* last = nullptr;

	while (CAN_BE_EXPRESSION(tok))
	{
		// printf("Debug: token = %s\n", tok.tok.tostr().c_str());
		// printf("Debug: last = ");
		// if (last) {last->print(); printf("\n");} else printf("nullptr\n");

	// Check for variable declaration expression
	if (tok.tok.type == toktype::KEYWORD && IS_ENUM_VARTYPE(tok.tok.keyword))
	{
		vartypes type = static_cast<vartypes>(tok.tok.keyword);
		string name;
		Nodes::Expression* value;

		tok = *tok.next;

		if (tok.tok.type == toktype::IDENTIFIER)
		{
			name = tok.tok.str;
			tok = *tok.next;

			if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::ASS))
			{
				value = parse_expression(tok, 1); // Skip the '='
			}
			else 
			{
				value = Nodes::getDefaultValueForType(type, tok.tok.position);
			}

			last = incRet(
				new Nodes::VarDeclExpression{pos, type, name, value},
				tok, 0);
			continue;
		}
		else error(tok, "Expected identifier as variable name after variable type");
	}
	// Number
	else if (tok.tok.type == toktype::NUM)
	{
		last = incRet(
			new Nodes::NumLiteralExpression{pos, tok.tok.num},
			tok, 1);
		continue;
	}
	// Boolean
	else if (tok.tok.type == toktype::KEYWORD && tok.tok.keyword == uenum(keywords::TRUE))
	{
		last = incRet(
			new Nodes::BoolLiteralExpression{pos, true},
			tok, 1);
		continue;
	}
	else if (tok.tok.type == toktype::KEYWORD && tok.tok.keyword == uenum(keywords::FALSE))
	{
		last = incRet(
			new Nodes::BoolLiteralExpression{pos, false},
			tok, 1);
		continue;
	}
	// Null
	else if (tok.tok.type == toktype::KEYWORD && tok.tok.keyword == uenum(keywords::_NULL))
	{
		last = incRet(
			new Nodes::NullLiteralExpression{pos},
			tok, 1);
		continue;
	}
	// String
	else if (tok.tok.type == toktype::STRING)
	{
		last = incRet(
			new Nodes::StringLiteralExpression{pos, tok.tok.str},
			tok, 1);
		continue;
	}
	// TODO: Ternary operator
	// Identifier, might be a function call or an assignment or simply a variable
	else if (tok.tok.type == toktype::IDENTIFIER)
	{
		// Parse function call
		if (tok.next->tok.type == toktype::OPERATOR && tok.next->tok.keyword == uenum(operators::LPAREN))
		{
			// Collect arguments
			string name = tok.tok.str;
			std::vector<Nodes::Expression*> args;
			tok = *tok.next->next;
			while (tok.tok.type != toktype::OPERATOR || tok.tok.keyword != uenum(operators::RPAREN))
			{
				args.push_back(parse_expression(tok, 0));
				// Check for a right parenthesis
				if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::RPAREN))
					break;
				// Account for the comma
				if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::COMMA))
					tok = *tok.next;
				// Account for EOF (if there's no right parenthesis)
				if (tok.tok.type == toktype::TOK_EOF)
					error(tok, "Expected ')' after function call");
			}
			// Increment the token to skip the closing parenthesis
			tok = *tok.next;

			last = incRet(
				new Nodes::FunctionCallExpression{pos, name, args},
				tok, 0);
			continue;
		}
		// Parse assignment
		else if (tok.next->tok.type == toktype::OPERATOR && tok.next->tok.keyword == uenum(operators::ASS))
		{
			string name = tok.tok.str;
			Nodes::Expression* value = parse_expression(tok, 2);
			last = incRet(
				new Nodes::AssignExpression{pos, name, value},
				tok, 0);
			continue;
		}
		// Parse special assignments (+=, --, *=, etc)
		else if (tok.next->tok.type == toktype::OPERATOR && isBinOp(tok.next->tok.keyword))
		{
			// Check if it's an assignment (e.g. +=) by checking if the next token is an '=' after a binary operator
			if (tok.next->next->tok.type == toktype::OPERATOR && tok.next->next->tok.keyword == uenum(operators::ASS))
			{
				string name = tok.tok.str;
				operators op = static_cast<operators>(tok.next->tok.keyword);
				size_t binExprPos = tok.next->tok.position;
				last = incRet(
					new Nodes::AssignExpression{pos, name, 
						new Nodes::BinaryExpression{binExprPos,
							new Nodes::IdentifierExpression{pos, name},
							op,
							parse_expression(tok, 3)}},
					tok, 0);
				continue;
			}
			// if there's a double operator after the identifier, it's an automatic bop
			else if (tok.next->next->tok.type == toktype::OPERATOR && tok.next->next->tok.keyword == tok.next->tok.keyword)
			{
				Nodes::Expression* value = new Nodes::BinaryExpression{pos, new Nodes::IdentifierExpression{pos, tok.tok.str}, static_cast<operators>(tok.next->next->tok.keyword), new Nodes::NumLiteralExpression{tok.tok.position, getValueForDoubleOp(tok.next->next->tok.keyword)}};
				last = incRet(
					new Nodes::AssignExpression{pos, tok.tok.str, value},
					tok, 3);
				continue;
			}
			// else it's probably a simple binary operation, it will be handled later on for now, return a variable
			else
			{
				last = incRet(
					new Nodes::IdentifierExpression{pos, tok.tok.str},
					tok, 1);
				continue;
			}
		}
		// It's probably just a variable
		else
		{
			last = incRet(
				new Nodes::IdentifierExpression{pos, tok.tok.str},
				tok, 1);
			continue;
		}
	}
	// Check for array literal or array access
	else if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::LBRACK))
	{
		// if last is IdentifierExpression or a literal, we'll assume it's an array access
		if (last && (IsType<Nodes::IdentifierExpression>(last) || IsType<Nodes::StringLiteralExpression>(last) || IsType<Nodes::ArrayLiteralExpression>(last)))
		{
			// Parse array access
			Nodes::Expression* index = parse_expression(tok, 1);
			// skip the closing bracket
			tok = *tok.next;

			last = incRet(
				new Nodes::ArrayAccessExpression{pos, last, index},
				tok, 0);
			continue;
		}
		// if it's a num or bool or bool literal return an error
		else if (last && (IsType<Nodes::NumLiteralExpression>(last) || IsType<Nodes::BoolLiteralExpression>(last) || IsType<Nodes::NullLiteralExpression>(last)))
		{
			error(tok, "Can't access an element of a non-iterable type");
		}

		// Parse array literal
		std::vector<Nodes::Expression*> elements;
		TokenNode* elem = tok.next;
		bool isRangeLiteral = false;
		// Check if there's a colon between the brackets by looping through the tokens until we find a closing bracket
		while (elem->tok.type != toktype::OPERATOR || elem->tok.keyword != uenum(operators::RBRACK))
		{
			// Check if it's a colon - it's a range literal
			if (elem->tok.type == toktype::OPERATOR && elem->tok.keyword == uenum(operators::COLON))
				{isRangeLiteral = true;
				break;}
			// Check if it's a comma = it's a normal array literal
			else if (elem->tok.type == toktype::OPERATOR && elem->tok.keyword == uenum(operators::COMMA))
				break;
			// increment the elem
			elem = elem->next;
		}

		// reset the elem to the first element
		elem = tok.next;

		// If it's a range literal
		if (isRangeLiteral)
		{
			// Parse the start and end of the range
			Nodes::Expression* start = parse_expression(*elem, 0);
			Nodes::Expression* end = parse_expression(*elem, 1); // skip the colon
			Nodes::Expression* step = new Nodes::NumLiteralExpression{pos, 1};
			// if we have another colon, set the step
			if (elem->tok.type == toktype::OPERATOR && elem->tok.keyword == uenum(operators::COLON))
			{
				delete step;
				step = parse_expression(*elem, 1); // skip the colon
			}
			
			// Make sure we have a closing bracket
			if (elem->tok.type != toktype::OPERATOR || elem->tok.keyword != uenum(operators::RBRACK))
				error(*elem, "Expected closing bracket");
			// skip the closing bracket
			tok = *(elem->next);

			last = incRet(
				new Nodes::RangeArrayLiteralExpression{pos, start, end, step},
				tok, 0);
			continue;
		}

		// Else it's a normal array literal
		while (elem->tok.type != toktype::OPERATOR || elem->tok.keyword != uenum(operators::RBRACK))
		{
			elements.push_back(parse_expression(*elem, 0));
			elem = elem->next;
			// Account for the comma
			if (elem->tok.type == toktype::OPERATOR && elem->tok.keyword == uenum(operators::COMMA))
				elem = elem->next;
		}
		
		// Make sure we have a closing bracket
		if (elem->tok.type != toktype::OPERATOR || elem->tok.keyword != uenum(operators::RBRACK))
			error(*elem, "Expected closing bracket");
		// Increment the token and skip the closing bracket
		tok = *(elem->next);

		last = incRet(
			new Nodes::ArrayLiteralExpression{pos, elements},
			tok, 0);
		continue;
	}
	// Check for parenthesis too
	else if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::LPAREN))
	{
		// Parse the expression inside the parenthesis
		Nodes::Expression* expr = parse_expression(tok, 1); // skip the opening parenthesis
		// Make sure we have a closing parenthesis
		if (tok.tok.type != toktype::OPERATOR || tok.tok.keyword != uenum(operators::RPAREN))
			error(tok, "Expected closing parenthesis");
		// Increment the token to skip the closing parenthesis
		tok = *(tok.next);

		last = incRet(
			new Nodes::ParenthesisExpression{pos,  expr},
			tok, 0);
		continue;
	}
	// Unary expression
	else if (tok.tok.type == toktype::OPERATOR && isUnOp(tok.tok.keyword))
	{
		// if last is a nullptr or an EmptyExpression, parse a unary expression
		if (!last || (last && IsType<Nodes::EmptyExpression>(last)))
		{
			auto op = static_cast<operators>(tok.tok.keyword);
			last = incRet(
				new Nodes::UnaryExpression{pos, op, parse_expression(tok, 1)},
				tok, 0);
			continue;
		}
		// else parse a binary expression or error
	}
	// Binary expression
	if (tok.tok.type == toktype::OPERATOR && isBinOp(tok.tok.keyword))
	{
		if (last)
		{
			auto op = static_cast<operators>(tok.tok.keyword);
			last = incRet(
				new Nodes::BinaryExpression{pos, last, op, parse_expression(tok, 1)},
				tok, 0);
			continue;
		} else error(tok, "Expected a value before binary operation %s (<value> %s <value>)", getStringFromId(tok.tok.keyword).c_str(), getStringFromId(tok.tok.keyword).c_str());
	}

	else
	{
		error(tok, "Unexpected token: %s", tok.tok.tostr().c_str());
	}

	}

	return last;
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
		return parse_function(tok, 1);
	case uenum(keywords::TRUE): case uenum(keywords::FALSE): // -----=====*****\ BOOL /*****=====-----
		return parse_expression_statement(tok, 1);
	default:
		// VARTYPES or ERROR
		if (IS_ENUM_VARTYPE(tok.tok.keyword))
			return parse_variable(tok, 0);
		else
			error(tok, "Unexpected keyword: %s", tok.tok.str.c_str());
	}

	return incRet(
		new Nodes::Statement{tok.tok.position},
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
		// Skip the '{'
		tok = *tok.next;
		// Parse until we reach the end of the block
		while (tok.tok.type != toktype::OPERATOR || tok.tok.keyword != uenum(operators::RBRACE))
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
				tok, 2);
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
					tok, 4);
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
				tok, 2);
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
					tok, 4);
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
			tok, 1);
	else
	{
		auto value = new Nodes::Return{pos, parse_expression(tok, 0)};

		if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::SEMICOLON))
			return incRet(value, tok, 1); // skip the semicolon
		else error(tok, "Expected ';' after return statement (return <expression>; or return;)");
	}

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
	init = parse_expression(tok);
	// for init; cond; inc body
	if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::SEMICOLON))
	{
		// Get the condition, skip the semicolon
		cond = parse_expression(tok, 1);
		// for init; cond; inc body
		if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::SEMICOLON))
		{
			// Get the increment (Skip the semicolon)
			inc = parse_expression(tok, 1);
			// Get the body
			body = parse_block(tok);
			return incRet(
				new Nodes::For{pos, init, cond, inc, body},
				tok, 0);
		}
		else error(tok, "Expected ';' in for statement (for <init>; <cond>; <inc>)");
	}
	// for init : INT body || for init : ITER body
	else if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::COLON))
	{
		// Get the ITER or the INT, skip the colon
		iterOrNum = parse_expression(tok, 1);
		// Get the body
		body = parse_block(tok);
		// Return
		return incRet(
			new Nodes::ForIter{pos, init, iterOrNum, body},
			tok, 0);
	}
	else error(tok, "Expected ';' or ':' in for statement (for <init>; <cond>; <inc> OR for <init> : <iteretable or max>)");

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

	// We should never reach this point
	return incRet(
		new Nodes::Break{pos},
		tok, 1);
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

	// We should never reach this point
	return incRet(
		new Nodes::Continue{pos},
		tok, 1);
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
			new Nodes::Ite{pos, condition, block, elseBlock},
			tok, 0);
	}
	else if (tok.tok.type == toktype::KEYWORD && tok.tok.keyword == uenum(keywords::ELIF))
	{
		size_t elif_pos = tok.tok.position;
		// parse the else if block
		auto elifStatement = parse_if(tok, 1);
		
		Nodes::StatementBlock* elifBlock = new Nodes::StatementBlock{elif_pos};
		elifBlock->statements.push_back(elifStatement);

		return incRet(
			new Nodes::Ite{pos, condition, block, elifBlock},
			tok, 0);
	}
	else return incRet(
		new Nodes::Ite{pos, condition, block, new Nodes::StatementBlock{tok.tok.position}},
		tok, 0);

	// We should never reach this point
	return incRet(
		new Nodes::Ite{pos, condition, new Nodes::StatementBlock{}, new Nodes::StatementBlock{}},
		tok, 1);
}
// TODO: inline Nodes::ClassDecl* Parser::parse_class(TokenNode& tok, int skip) const
inline Nodes::NamespaceDecl* Parser::parse_namespace(TokenNode& tok, int skip) const
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
			new Nodes::NamespaceDecl{pos, name, block},
			tok, 0);
	}
	else error(tok, "Expected identifier namespace name (namespace <name> { ... })");

	// We should never reach this point
	return incRet(
		new Nodes::NamespaceDecl{pos, "", new Nodes::StatementBlock{}},
		tok, 1);
}
inline Nodes::FunctionDecl* Parser::parse_function(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	string name;
	map<pair<vartypes, string>, Nodes::Expression*> params;
	vartypes rType = vartypes::VAR;
	Nodes::StatementBlock* body;

	// TODO: Maybe add support to lambda like functions, support syntax like this - fun foo(a, b) = (a + b);

	// fun <name> ( <args> ) : <rType> { <body> }
	if (tok.tok.type == toktype::IDENTIFIER)
	{
		// Get the function name
		name = tok.tok.str;
		tok = *tok.next;
		// Get the arguments
		if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::LPAREN))
		{
			tok = *tok.next;
			while (tok.tok.type != toktype::OPERATOR || tok.tok.keyword != uenum(operators::RPAREN))
			{
				pair<pair<vartypes, string>, Nodes::Expression*> param;
				param.second = new Nodes::EmptyExpression{tok.tok.position};

				// param type = VAR
				if (tok.tok.type == toktype::IDENTIFIER)
				{
					param.first.first = vartypes::VAR;
					param.first.second = tok.tok.str;
					tok = *tok.next;
				}
				// Get param type
				else if (tok.tok.type == toktype::KEYWORD && IS_ENUM_VARTYPE(tok.tok.keyword))
				{
					param.first.first = static_cast<vartypes>(tok.tok.keyword);
					tok = *tok.next;
					if (tok.tok.type == toktype::IDENTIFIER)
					{
						param.first.second = tok.tok.str;
						tok = *tok.next;
					}
					else error(tok, "Expected identifier after type (type <name>) in parameter declaration");
				}
				else error(tok, "Expected parameter type or identifier (parameter name) if no type is specified var is assumed");

				// Get the default value is exists
				if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::EQ))
				{
					tok = *tok.next;
					param.second = parse_expression(tok);
				}

				// Add the parameter to the list
				params.insert(param);

				// Check if we reached the end of the parameters
				if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::RPAREN))
				{
					break;
				}
				// Check if we reached the end of this parameter
				else if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::COMMA))
				{
					tok = *tok.next;
				}
				else error(tok, "Expected ',' or ')' after parameter declaration");
			}

			// skip the ')'
			tok = *tok.next;

			// Get the return type if exists, otherwise assume var
			if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::COLON))
			{
				tok = *tok.next;
				if (tok.tok.type == toktype::KEYWORD && IS_ENUM_VARTYPE(tok.tok.keyword))
				{
					rType = static_cast<vartypes>(tok.tok.keyword);
					tok = *tok.next;
				}
				else error(tok, "Expected return type ( : <type>)");
			}

			// Get the function body
			body = parse_block(tok);

			// Return the function
			return incRet(
				new Nodes::FunctionDecl{pos, name, params, rType, body},
				tok, 0);
		}
		else error(tok, "Expected '(' after function name");
	}
	else error(tok, "Expected identifier function name (fun <name>( <args> ) : <rType> { ... })");
	
	// We should never reach this point
	return incRet(
		new Nodes::FunctionDecl{pos, name, params, rType, body},
		tok, 1);
}
inline Nodes::VarDecl* Parser::parse_variable(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	string name;
	vartypes type;
	Nodes::Expression* value;

	// <type> <name> = <value>
	if (tok.tok.type == toktype::KEYWORD && IS_ENUM_VARTYPE(tok.tok.keyword))
	{
		// Get the variable type
		type = static_cast<vartypes>(tok.tok.keyword);
		tok = *tok.next;
		if (tok.tok.type == toktype::IDENTIFIER)
		{
			// Get the variable name
			name = tok.tok.str;
			tok = *tok.next;

			// Get the variable value if exists
			if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::ASS))
			{
				tok = *tok.next;
				value = parse_expression(tok);

				if (tok.tok.type != toktype::OPERATOR || tok.tok.keyword != uenum(operators::SEMICOLON))
					error(tok, "Expected ';' after variable initialization (<type> <name> = <value>;)");

				return incRet(
					new Nodes::VarDecl{pos, type, name, value},
					tok, 1); // skip the semicolon
			}
			// If there is no value, look for a semicolon
			else if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::SEMICOLON))
			{
				tok = *tok.next;
				// Set the default value to whatever the type's default is
				value = Nodes::getDefaultValueForType(type, pos);

				return incRet(
					new Nodes::VarDecl{pos, type, name, value},
					tok, 0);
			}
			else error(tok, "Expected '=' or ';' after variable declaration");
		}
		else error(tok, "Expected identifier after type (<type> <name> = <value>;)");
	}
	else error(tok, "Expected variable type (<type> <name> = <value>;)");

	// We should never reach this point
	return incRet(
		new Nodes::VarDecl{pos, type, name, value},
		tok, 1);
}

inline Nodes::Statement* Parser::parse_expression_statement(TokenNode& tok, int skip) const
{
	size_t pos = tok.tok.position;

	for (int i = 0; i < skip; i++)
	{
		tok = *tok.next;
	}

	Nodes::Statement* expr = new Nodes::ExpressionStatement{pos, parse_expression(tok)};

	if (tok.tok.type == toktype::OPERATOR && tok.tok.keyword == uenum(operators::SEMICOLON))
	{
		return incRet(expr, tok, 1); // skip the semicolon
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