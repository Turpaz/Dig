#include "grammar.hpp"

unsigned getKeywordOrVartypeFromString(string s)
{
	#define KEYWORD(id, str) if (s == str) return uenum(keywords::id);
	#define VARTYPE(id, str) if (s == str) return uenum(vartypes::id);
	#define OPERATOR(id, str)
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
	#define OPERATOR(id, str) if (i == uenum(operators::id)) return str;
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	return "Unrecognized ID";
}