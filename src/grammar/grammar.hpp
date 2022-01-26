#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <string>
#include <vector>

using std::string;

enum class toktype : char
{
	IDENTIFIER,
	STRING,

	NUM,

	KEYWORD,

	TOK_EOF,
};

enum class keywords : unsigned
{
	SETTING_START_VALUE_IGNORE_THIS = 0,
	#define KEYWORD(id, str) id,
	#define VARTYPE(id, str)
	#define OPERATOR(id, str)
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	GETTING_COUNT_IGNORE_THIS
};
enum class vartypes : unsigned
{
	SETTING_START_VALUE_IGNORE_THIS = static_cast<unsigned>(keywords::GETTING_COUNT_IGNORE_THIS),
	#define KEYWORD(id, str)
	#define VARTYPE(id, str) id,
	#define OPERATOR(id, str)
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	GETTING_COUNT_IGNORE_THIS
};
enum class operators : unsigned
{
	SETTING_START_VALUE_IGNORE_THIS = static_cast<unsigned>(vartypes::GETTING_COUNT_IGNORE_THIS),
	#define KEYWORD(id, str)
	#define VARTYPE(id, str)
	#define OPERATOR(id, str) id,
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	GETTING_COUNT_IGNORE_THIS
};

unsigned getKeywordOrVartypeFromString(string s);
string getStringFromId(unsigned i);

#endif // GRAMMAR_HPP