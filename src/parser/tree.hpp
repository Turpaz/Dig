#ifndef PARSER_TREE_HPP
#define PARSER_TREE_HPP

#include <vector>
#include <string>
#include <map>
#include "../grammar/grammar.hpp"

using std::vector;
using std::string;
using std::map;

struct Statement { size_t position; };
struct Expression { size_t position; };

struct StatementBlock : public Statement // A block of statements { ... }
{
	vector<Statement*> statements;
};
struct Ite : public Statement // if condition { ifBranch } else { elseBranch }
{
	Expression condition;
	StatementBlock ifBranch;
	StatementBlock elseBranch;
};
struct VarDecl : public Statement // type name = value;
{
	vartypes type;
	string name;
	Expression value; // if value is not specified, it is set to default (0, "", etc.)
};
struct FunctionDecl : public Statement // fun name(args) { body }
{
	string name;
	map<vartypes, string> args;
	vartypes rType; // if not specified, it is set to vartypes::VAR
	StatementBlock body;
};
struct For : public Statement /* for init; condition; step || for init : INT e.g. for int i : [0:10:1] { ... } || for int i : 10 { ... } || for int i = 0; i < 10; i++ { ... } */
{
	VarDecl init;
	Expression condition;
	Expression step;
	StatementBlock body;
};
struct ForIter : public Statement // for init : iter { body } e.g: for int i : [0:10:1] { ... }, for char c : "hello" { ... }
{
	VarDecl init;
	Expression iter;
	StatementBlock body;
};
struct While : public Statement // while condition { body }
{
	Expression condition;
	StatementBlock body;
};
struct Return : public Statement // return value;
{
	Expression value;
};
struct Import : public Statement // import math; TODO: maybe add a way to import a specific function or class
{
	string name;
	string as; // default is the same as name
};
struct Break : public Statement // break;
{

};
struct Continue : public Statement // continue;
{

};
struct Assign : public Statement // Assign a value to a variable, a = 1293; a += 12; a++;
{
	string name;
	Expression value;
};
struct ExpressionStatement : public Statement // Simple expression by themself e.g. a + b; foo();
{
	Expression value;
};
struct BinaryExpression : public Expression // Simple arithmetics actions: left [op] right; (op + - * / % ^)
{
	Expression left;
	Expression right;
	string op;
};
struct UnaryExpression : public Expression // negative/positive/not: -value, !value
{
	Expression value;
	string op;
};
struct FunctionCallExpression : public Expression // Call a function (return something maybe null): name(args);
{
	string name;
	vector<Expression> args;
};
struct ArrayAccessExpression : public Expression // Access array element: array[index]
{
	Expression array;
	Expression index;
};
struct IdentifierExpression : public Expression // Access Variable: name
{
	string name;
};
struct ArrayLiteralExpression : public Expression // Array literal: [1, 2, 3], [0:10:1]
{
	vector<Expression> values;
};
struct StringLiteralExpression : public Expression // String literal: "Hello World"
{
	string value;
};
struct NumLiteralExpression : public Expression // Number literal: 12, 0.12, .12, 12.
{
	double value;
};

#endif // PARSER_TREE_HPP