#include "grammar.hpp"

unsigned getKeywordOrVartypeFromString(string s)
{
	#define KEYWORD(id, str) if (s == str) return static_cast<unsigned>(keywords::id);
	#define VARTYPE(id, str) if (s == str) return static_cast<unsigned>(vartypes::id);
	#define OPERATOR(id, str)
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	return 0;
}