#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <cstdint>

using std::string;

enum class toktype : char;

struct Token
{
	union { string str; long double num; unsigned keyword; };
	toktype type;
	size_t position;
};

enum class toktype : char
{
	KEYWORD,
	VARTYPE,

	OPERATOR,
	SEPARATOR,

	IDENTIFIER,
	NUM,
	STRING,

	TOK_EOF,
};

#endif