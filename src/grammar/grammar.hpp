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
	OPERATOR,

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

// tools to differentiate between a keyword, a vartype and an operator
#define FIRST_KEYWORD_ID static_cast<unsigned>(keywords::SETTING_START_VALUE_IGNORE_THIS + 1)
#define FIRST_VARTYPE_ID static_cast<unsigned>(vartypes::SETTING_START_VALUE_IGNORE_THIS + 1)
#define FIRST_OPERATOR_ID static_cast<unsigned>(operators::SETTING_START_VALUE_IGNORE_THIS + 1)

unsigned getKeywordOrVartypeFromString(string s);
string getStringFromId(unsigned i);

#endif // GRAMMAR_HPP