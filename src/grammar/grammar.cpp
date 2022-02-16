#include "grammar.hpp"

unsigned getKeywordOrVartypeFromString(string s)
{
	#define KEYWORD(id, str) if (s == str) return uenum(keywords::id);
	#define VARTYPE(id, str) if (s == str) return uenum(vartypes::id);
	#define OPERATOR(id, str, type)
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	return 0;
}

string getStringFromId(unsigned i)
{
	#define KEYWORD(id, str) if (i == uenum(keywords::id)) return str;
	#define VARTYPE(id, str) if (i == uenum(vartypes::id)) return str;
	#define OPERATOR(id, str, type) if (i == uenum(operators::id)) return str;
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	return "Unrecognized ID";
}

bool isBinOp(unsigned int u)
{
	#define KEYWORD(id, str)
	#define VARTYPE(id, str)
	#define OPERATOR(id, str, type) if ((u == uenum(operators::id)) && (type == LANG_BIN_OP || type == LANG_BIN_UN_OP)) return true;
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	return false;
}
bool isUnOp(unsigned int u)
{
	#define KEYWORD(id, str)
	#define VARTYPE(id, str)
	#define OPERATOR(id, str, type) if ((u == uenum(operators::id)) && (type == LANG_UN_OP || type == LANG_BIN_UN_OP)) return true;
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	return false;
}

double getValueForDoubleOp(unsigned binOp)
{
	switch (static_cast<operators>(binOp))
	{
	case operators::PLUS:
		return 1;
	case operators::MINUS:
		return 1;
	case operators::MUL:
		return 2;
	case operators::DIV:
		return 2;
	case operators::MOD:
		return 2;
	case operators::POW:
		return 2;
	default:
		fprintf(stderr, "compiler code error: called getValueForDoubleOp with an invalid operator\n");
		exit(-400);
	}
}
