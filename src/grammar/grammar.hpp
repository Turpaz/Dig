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
	ROOT,
};

enum class keywords : unsigned
{
	__BEGIN = 0,
	#define KEYWORD(id, str) id,
	#define VARTYPE(id, str)
	#define OPERATOR(id, str)
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	__END
};
enum class vartypes : unsigned
{
	__BEGIN = static_cast<unsigned>(keywords::__END),
	#define KEYWORD(id, str)
	#define VARTYPE(id, str) id,
	#define OPERATOR(id, str)
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	__END
};
enum class operators : unsigned
{
	__BEGIN = static_cast<unsigned>(vartypes::__END),
	#define KEYWORD(id, str)
	#define VARTYPE(id, str)
	#define OPERATOR(id, str) id,
	#include "language.inc"
	#undef KEYWORD
	#undef VARTYPE
	#undef OPERATOR
	__END
};

inline constexpr unsigned uenum(keywords k) { return static_cast<unsigned>(k); }
inline constexpr unsigned uenum(vartypes v) { return static_cast<unsigned>(v); }
inline constexpr unsigned uenum(operators o) { return static_cast<unsigned>(o); }
inline constexpr char cenum(toktype t) { return static_cast<char>(t); }

// tools to differentiate between a keyword, a vartype and an operator
#define FIRST_KEYWORD_ID (uenum(keywords::__BEGIN) + 1)
#define LAST_KEYWORD_ID (uenum(keywords::__END) - 1)
#define FIRST_VARTYPE_ID (uenum(vartypes::__BEGIN) + 1)
#define LAST_VARTYPE_ID (uenum(vartypes::__END) - 1)
#define FIRST_OPERATOR_ID (uenum(operators::__BEGIN) + 1)
#define LAST_OPERATOR_ID (uenum(operators::__END) - 1)

unsigned getKeywordOrVartypeFromString(string s);
string getStringFromId(unsigned i);

#endif // GRAMMAR_HPP