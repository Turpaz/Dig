#ifndef PARSER_TREE_HPP
#define PARSER_TREE_HPP

#include <vector>
#include <string>
#include <map>
#include "../grammar/grammar.hpp"

using std::vector;
using std::string;
using std::map;
using std::pair;

namespace Nodes
{
enum class Access : char
{
	PUBLIC,
	PRIVATE,
	PROTECTED,
	NONE,
};

struct Statement
{
	size_t position; 

	Statement(size_t position) : position(position) {};
	Statement() : position(-1) {};

	virtual void print() const { printf("Statement at %zu\n", position); }

	virtual void codegen(Codegen& codegen) const;
};
struct Expression
{
	size_t position;
	Expression(size_t position) : position(position) {};
	Expression() : position(-1) {};
	
	virtual void print() const { printf("Expression at %zu\n", position); }

	// virtual string codegen() const { return "; Just an Expression"; }
};

struct StatementBlock : public Statement // A block of statements { ... }
{
	vector<Statement*> statements;

	StatementBlock(size_t position, vector<Statement*> statements) : Statement(position), statements(statements) {}
	StatementBlock(size_t position) : Statement(position), statements() {}
	StatementBlock() : Statement(0), statements() {}
	~StatementBlock() { for (auto& statement : statements) delete statement; }

	void print() const
	{
		printf("(StatementBlock at %zu)\n", position);
		this->printBlock();
	}

	void printBlock() const
	{
		printf("{\n");
		for (auto& statement : statements)
		{
			printf("\t");
			statement->print();
		}
		printf("}\n");
	}

	void codegen(Codegen& codegen) const;
};
struct Ite : public Statement // if condition { ifBranch } else { elseBranch }
{
	Expression* condition;
	StatementBlock* ifBranch;
	StatementBlock* elseBranch;

	Ite(size_t position, Expression* condition, StatementBlock* ifBranch, StatementBlock* elseBranch) : Statement(position), condition(condition), ifBranch(ifBranch), elseBranch(elseBranch) {}

	void print() const
	{
		printf("(Ite at %zu)\nif ", position);
		condition->print();
		printf("\n");
		ifBranch->printBlock();
		printf("else\n");
		elseBranch->printBlock();
	}

	void codegen(Codegen& codegen) const;
};
struct VarDecl : public Statement // type name = value; type name;
{
	vartypes type;
	string name;
	Expression* value; // if value is not specified, it is set to default (0, "", etc.)

	VarDecl(size_t position, vartypes type, string name, Expression* value) : Statement(position), type(type), name(name), value(value) {}

	void print() const
	{
		printf("(VarDecl at %zu)\n%s %s = ", position, getStringFromId(uenum(type)).c_str(), name.c_str());
		value->print();
		printf(";\n");
	}

	void codegen(Codegen& codegen) const;
};
struct FunctionDecl : public Statement // fun name(args) { body }
{
	string name;
	map<pair<vartypes, string>, Expression*> args;
	vartypes rType; // if not specified, it is set to vartypes::VAR
	StatementBlock* body;

	FunctionDecl(size_t position, string name, map<pair<vartypes, string>, Expression*> args, vartypes rType, StatementBlock* body) : Statement(position), name(name), args(args), rType(rType), body(body) {}

	void print() const
	{
		printf("(FunctionDecl at %zu)\nfun %s(", position, name.c_str());

		if (!args.empty()) printf("\n  ");
		for (auto& arg : args)
		{
			printf("%s %s = ", getStringFromId(uenum(arg.first.first)).c_str(), arg.first.second.c_str());
			arg.second->print();
			printf(", ");
		}
		printf(") : %s\n", getStringFromId(uenum(rType)).c_str());
		body->printBlock();
	}

	void codegen(Codegen& codegen) const;
};
struct ClassSysFunctionDecl : public Statement
{
	string name; // initialize, terminate, __str__, __OP_PLUS__, etc.
	map<pair<vartypes, string>, Expression*> args;
	vartypes rType;
	StatementBlock* body;

	ClassSysFunctionDecl(size_t position, string name, map<pair<vartypes, string>, Expression*> args, vartypes rType, StatementBlock* body) : Statement(position), name(name), args(args), rType(rType), body(body) {}

	void print() const
	{
		printf("(ClassSysFunctionDecl at %zu)\n%s(", position, name.c_str());
		for (auto& arg : args)
		{
			printf("%s %s = ", getStringFromId(uenum(arg.first.first)).c_str(), arg.first.second.c_str());
			arg.second->print();
			printf(", ");
		}
		printf(")\n");
		body->printBlock();
	}

	void codegen(Codegen& codegen) const;
};
struct ClassDecl : public Statement // class name { body }
{
	string name; // TODO: add inheritance
	map<VarDecl*, Access> members;
	map<FunctionDecl*, Access> functions;
	vector<ClassSysFunctionDecl*> sysFunctions; // All public functions, initialize, terminate, __str__, __OP_PLUS__, etc.

	ClassDecl(size_t position, string name, map<VarDecl*, Access> members, map<FunctionDecl*, Access> functions, vector<ClassSysFunctionDecl*> sysFunctions) : Statement(position), name(name), members(members), functions(functions), sysFunctions(sysFunctions) {}

	void print() const
	{
		printf("(ClassDecl at %zu)\nclass %s\n{\n", position, name.c_str());
		for (auto& member : members)
		{
			printf("\t");
			member.first->print();
		}
		for (auto& function : functions)
		{
			printf("\t");
			function.first->print();
			printf("\n");
		}
		for (auto& function : sysFunctions)
		{
			printf("\t");
			function->print();
			printf("\n");
		}
		printf("}\n");
	}

	void codegen(Codegen& codegen) const;
};
struct NamespaceDecl : public Statement // namespace name { body }
{
	string name;
	StatementBlock* body;

	NamespaceDecl(size_t position, string name, StatementBlock* body) : Statement(position), name(name), body(body) {}

	void print() const
	{
		printf("(NamespaceDecl at %zu)\nnamespace %s\n", position, name.c_str());
		body->printBlock();
	}

	void codegen(Codegen& codegen) const;
};
struct For : public Statement /* for init; condition; step e.g. for int i; i < 10; i++ { ... } */
{
	Expression* init;
	Expression* condition;
	Expression* step;
	StatementBlock* body;

	For(size_t position, Expression* init, Expression* condition, Expression* step, StatementBlock* body) : Statement(position), init(init), condition(condition), step(step), body(body) {}

	void print() const
	{
		printf("(For at %zu)\nfor ", position);
		init->print();
		printf("; ");
		condition->print();
		printf("; ");
		step->print();
		printf("\n");
		body->printBlock();
	}

	void codegen(Codegen& codegen) const;
};
struct ForIter : public Statement // for init : iterOrNum { body } e.g: for int i : [0:10:1] { ... }, for int i : 10 { ... }
{
	Expression* init;
	Expression* iterOrNum;
	StatementBlock* body;

	ForIter(size_t position, Expression* init, Expression* iterOrNum, StatementBlock* body) : Statement(position), init(init), iterOrNum(iterOrNum), body(body) {}

	void print() const
	{
		printf("(ForIter at %zu)\nfor ", position);
		init->print();
		printf(" : ");
		iterOrNum->print();
		printf("\n");
		body->printBlock();
	}

	void codegen(Codegen& codegen) const;
};
struct While : public Statement // while condition { body }
{
	Expression* condition;
	StatementBlock* body;

	While(size_t position, Expression* condition, StatementBlock* body) : Statement(position), condition(condition), body(body) {}

	void print() const
	{
		printf("(While at %zu)\nwhile ", position);
		condition->print();
		printf("\n");
		body->printBlock();
	}

	void codegen(Codegen& codegen) const;
};
struct Return : public Statement // return value;
{
	Expression* value;

	Return(size_t position, Expression* value) : Statement(position), value(value) {}

	void print() const
	{
		printf("(Return at %zu)\nreturn ", position);
		value->print();
		printf(";\n");
	}

	void codegen(Codegen& codegen) const;
};
struct ImportModule : public Statement // import math; import random as rdm; TODO: maybe add a way to import a specific function or class from the library
{
	string name;
	string as; // default is the same as name

	ImportModule(size_t position, string name, string as) : Statement(position), name(name), as(as) {}

	void print() const
	{
		printf("(ImportModule at %zu)\nimport %s", position, name.c_str());
		printf(" as %s;\n", as.c_str());
	}

	void codegen(Codegen& codegen) const;
};
struct ImportFile : public Statement // import "src/file.dg"; import "constatnts.dg" as consts; TODO: maybe add a way to import a specific function or class from the file
{
	string path;
	string as; // default is the same as name

	ImportFile(size_t position, string path, string as) : Statement(position), path(path), as(as) {}

	void print() const
	{
		printf("(ImportFile at %zu)\nimport \"%s\"", position, path.c_str());
		printf(" as %s;\n", as.c_str());
	}

	void codegen(Codegen& codegen) const;
};
struct Break : public Statement // break;
{
	Break(size_t position) : Statement(position) {}

	void print() const
	{
		printf("(Break at %zu)\nbreak;\n", position);
	}

	void codegen(Codegen& codegen) const;
};
struct Continue : public Statement // continue;
{
	Continue(size_t position) : Statement(position) {}

	void print() const
	{
		printf("(Continue at %zu)\ncontinue;\n", position);
	}

	void codegen(Codegen& codegen) const;
};
struct RootStatement : public Statement // RootStatement is just the first node
{
	RootStatement(size_t position) : Statement(position) {}

	void print() const
	{
		printf("(RootStatement at %zu)\n", position);
	}

	void codegen(Codegen& codegen) const;
};
struct EofStatement : public Statement // EofStatement is just the last node
{
	EofStatement(size_t position) : Statement(position) {}

	void print() const
	{
		printf("(EofStatement at %zu)\n", position);
	}

	void codegen(Codegen& codegen) const;
};
struct EmptyStatement : public Statement // ;
{
	EmptyStatement(size_t position) : Statement(position) {}

	void print() const
	{
		printf("(EmptyStatement at %zu)\n;\n", position);
	}

	void codegen(Codegen& codegen) const;
};
struct ExpressionStatement : public Statement // Simple expression by themself e.g. a + b; foo();
{
	Expression* value;

	ExpressionStatement(size_t position, Expression* value) : Statement(position), value(value) {}

	void print() const
	{
		printf("(ExpressionStatement at %zu)\n", position);
		value->print();
		printf(";\n");
	}

	void codegen(Codegen& codegen) const;
};
struct BinaryExpression : public Expression // Simple arithmetics actions: left [op] right; (+ - * / % ^ == != < > <= >=) e.g. a + b;
{
	Expression* left;
	Expression* right;
	operators op;

	BinaryExpression(size_t position, Expression* left, operators op, Expression* right) : Expression(position), left(left), right(right), op(op) {}

	void print() const
	{
		printf("(BinaryExpression at %zu) ", position);
		left->print();
		printf(" %s ", getStringFromId(uenum(op)).c_str());
		right->print();
	}
};
struct AssignExpression : public Expression // Assign a value to a variable, a = 1293; a += 12; a++;
{
	string name;
	Expression* value;

	AssignExpression(size_t position, string name, Expression* value) : Expression(position), name(name), value(value) {}

	void print() const
	{
		printf("(Assign at %zu)\n%s = ", position, name.c_str());
		value->print();
		printf(";\n");
	}
};
struct UnaryExpression : public Expression // negative/not: -value, !value // Maybe add ~
{
	Expression* value;
	operators op;

	UnaryExpression(size_t position, operators op, Expression* value) : Expression(position), value(value), op(op) {}

	void print() const
	{
		printf("(UnaryExpression at %zu) ", position);
		printf("%s", getStringFromId(uenum(op)).c_str());
		value->print();
		printf("\n");
	}
};
struct ParenthesisExpression : public Expression // (value)
{
	Expression* value;

	ParenthesisExpression(size_t position, Expression* value) : Expression(position), value(value) {}

	void print() const
	{
		printf("(ParenthesisExpression at %zu) ", position);
		printf("( ");
		value->print();
		printf(" )\n");
	}
};
struct TernaryExpression : public Expression // condition ? true : false
{
	Expression* condition;
	Expression* true_value;
	Expression* false_value;

	TernaryExpression(size_t position, Expression* condition, Expression* true_value, Expression* false_value) : Expression(position), condition(condition), true_value(true_value), false_value(false_value) {}

	void print() const
	{
		printf("(TernaryExpression at %zu) ", position);
		condition->print();
		printf(" ? ");
		true_value->print();
		printf(" : ");
		false_value->print();
		printf("\n");
	}
};
struct FunctionCallExpression : public Expression // Call a function (return something maybe null): name(args);
{
	string name;
	vector<Expression*> args;

	FunctionCallExpression(size_t position, string name, vector<Expression*> args) : Expression(position), name(name), args(args) {}

	void print() const
	{
		printf("(FunctionCallExpression at %zu) ", position);
		printf("%s(", name.c_str());
		for (auto& arg : args)
		{
			arg->print();
			printf(", ");
		}
		printf(")\n");
	}
};
struct VarDeclExpression : public Expression // VerDecl is a variable declaration
{
	vartypes type;
	string name;
	Expression* value; // if value is not specified, it is set to default (0, "", etc.)

	VarDeclExpression(size_t position, vartypes type, string name, Expression* value) : Expression(position), type(type), name(name), value(value) {}

	void print() const
	{
		printf("(VarDeclExpression at %zu)\n%s %s = ", position, getStringFromId(uenum(type)).c_str(), name.c_str());
		value->print();
		printf(";\n");
	}
};
struct ArrayAccessExpression : public Expression // Access array element: array[index]
{
	Expression* array;
	Expression* index;

	ArrayAccessExpression(size_t position, Expression* array, Expression* index) : Expression(position), array(array), index(index) {}

	void print() const
	{
		printf("(ArrayAccessExpression at %zu) ", position);
		array->print();
		printf("[");
		index->print();
		printf("]\n");
	}
};
struct MemberAccessExpression : public Expression // Call a member of an object: object.name;
{
	Expression* object;
	string name;

	MemberAccessExpression(size_t position, Expression* object, string name) : Expression(position), object(object), name(name) {}

	void print() const
	{
		printf("(MemberExpression at %zu) ", position);
		object->print();
		printf(".%s\n", name.c_str());
	}
};
struct IdentifierExpression : public Expression // Access Variable: name
{
	string name;

	IdentifierExpression(size_t position, string name) : Expression(position), name(name) {}

	void print() const
	{
		printf("(IdentifierExpression at %zu) ", position);
		printf("%s", name.c_str());
	}
};
struct ArrayLiteralExpression : public Expression // Array literal: [1, 2, 3]
{
	vector<Expression*> values;

	ArrayLiteralExpression(size_t position, vector<Expression*> values) : Expression(position), values(values) {}

	void print() const
	{
		printf("(ArrayLiteralExpression at %zu) ", position);
		printf("[");
		for (auto& value : values)
		{
			value->print();
			printf(", ");
		}
		printf("]\n");
	}
};
struct RangeArrayLiteralExpression : public Expression // Range array literal: [0:10:1]
{
	Expression* start;
	Expression* end;
	Expression* step;

	RangeArrayLiteralExpression(size_t position, Expression* start, Expression* end, Expression* step) : Expression(position), start(start), end(end), step(step) {}

	void print() const
	{
		printf("(RangeArrayLiteralExpression at %zu) ", position);
		printf("[");
		start->print();
		printf(":");
		end->print();
		printf(":");
		step->print();
		printf("]\n");
	}
};
struct StringLiteralExpression : public Expression // String literal: "Hello World"
{
	string value;

	StringLiteralExpression(size_t position, string value) : Expression(position), value(value) {}

	void print() const
	{
		printf("(StringLiteralExpression at %zu) ", position);
		printf("\"%s\"\n", value.c_str());
	}
};
struct NumLiteralExpression : public Expression // Number literal: 12, 0.12, .12, 12.
{
	double value;

	NumLiteralExpression(size_t position, double value) : Expression(position), value(value) {}

	void print() const
	{
		printf("(NumLiteralExpression at %zu) ", position);
		printf("%f\n", value);
	}
};
struct BoolLiteralExpression : public Expression // Boolean literal: true, false
{
	bool value;

	BoolLiteralExpression(size_t position, bool value) : Expression(position), value(value) {}

	void print() const
	{
		printf("(BoolLiteralExpression at %zu) ", position);
		printf("%s\n", value ? "true" : "false");
	}
};
struct NullLiteralExpression : public Expression // Null literal: null
{
	NullLiteralExpression(size_t position) : Expression(position) {}

	void print() const
	{
		printf("(NullLiteralExpression at %zu) ", position);
		printf("null\n");
	}
};
struct EmptyExpression : public Expression // ;
{
	EmptyExpression(size_t position) : Expression(position) {}

	void print() const
	{
		printf("(EmptyExpression at %zu) ", position);
		printf("\n");
	}
};

inline Expression* getDefaultValueForType(vartypes type, size_t pos)
{
	switch (type)
	{
	case vartypes::BOOL:
		return new BoolLiteralExpression{pos, false};
	case vartypes::INT: case vartypes::FLOAT:
		return new NumLiteralExpression{pos, 0};
	case vartypes::STR:
		return new StringLiteralExpression{pos, ""};
	case vartypes::VAR:
		return new NullLiteralExpression{pos};
	case vartypes::ARR:
		return new ArrayLiteralExpression{pos, {}};
	default:
		return new NullLiteralExpression{pos};
	}
}
}

#endif // PARSER_TREE_HPP