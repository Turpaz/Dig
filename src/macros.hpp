#ifndef MACROS_HPP
#define MACROS_HPP

#define DIG_VERSION "1.0.0"

#define USAGE_HELPER "Usage: dig input_file_path [options]\n\texamples:\n\
\t\tdig main.dg -o main.exe\n\
\t\tdig main.dg\n\
\t\tdig -h\n\
Options:\n\
\t-o <file>\t\t\tSet output file to <file>, the default will be the file name but with .exe extension.\n\
\t-h\t\t\t\tShows this message.\n\
\t-v\t\t\t\tShows the version of the compiler.\n\
\n\
\t-E\t\t\tPreprocess only.\n\
"

// Max 32 command line arguments
#define CMD_OPTIONS_STRING "o:hve" /* all the options */
class cmd_args
{
public:
	static const int None	= 0;
	static const int _o		= (1 << 0);
	static const int _h		= (1 << 1);
	static const int _v		= (1 << 2);
	static const int _E		= (1 << 3);
};

#define ERROR_WE_DONT_KNOW "Something went horribly wrong, probably a bug in the compiler itself, sorry"
#define UNEXPECTED_CHARACTER(ch) "We reached an unexpected character: '%c'", ch
#define ERROR_TWO_FLOAT_DOTS "Float number has two or more dots, it should only have one"
#define ERROR_NO_MATCHING_QUOTE "No closing double quotes for this pair, add it somewhere"
#define ERROR_CHAR_TOO_LONG "Single quotes are meant for single character literals, more were given"

#define IS_HEXA_DIGIT(c) ( (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') )
#define CAN_BE_OPERATOR(c) ( (c >= 33 && c <= 47 && c != 34 && c != 39)\
|| (c >= 58 && c <= 64)\
|| (c >= 91 && c <= 96 && c != 92)\
|| (c >= 123 && c <= 126))
#define GET_JUST_FILENAME(path) path.substr(path.find_last_of('/')+1, path.find_last_of('.')-(path.find_last_of('/')+1))

#define IS_ENUM_KEYWORD(u) (u >= FIRST_KEYWORD_ID && u <= LAST_KEYWORD_ID)
#define IS_ENUM_VARTYPE(u) (u >= FIRST_VARTYPE_ID && u <= LAST_VARTYPE_ID)
#define IS_ENUM_OPERATOR(u) (u >= FIRST_OPERATOR_ID && u <= LAST_OPERATOR_ID)

#define LANG_BIN_OP 0
#define LANG_UN_OP 1
#define LANG_BIN_UN_OP 2
#define LANG_OTHER_OP 3

template <class T, class Instance> inline bool IsType(const Instance* src)
{
	return dynamic_cast<const T*>(src) != nullptr;
}

#define CAN_BE_EXPRESSION(tok) \
		(tok.tok.type == toktype::IDENTIFIER || \
		tok.tok.type == toktype::STRING || \
		tok.tok.type == toktype::NUM || \
		(tok.tok.type == toktype::OPERATOR && \
		(isBinOp(tok.tok.keyword) || isUnOp(tok.tok.keyword)) || \
		tok.tok.keyword == uenum(operators::ASS)  || \
		tok.tok.keyword == uenum(operators::LBRACK) || tok.tok.keyword == uenum(operators::LPAREN)) || \
		(tok.tok.type == toktype::KEYWORD && \
		((tok.tok.keyword == uenum(keywords::TRUE) || tok.tok.keyword == uenum(keywords::FALSE) || tok.tok.keyword == uenum(keywords::_NULL)))\
		|| IS_ENUM_VARTYPE(tok.tok.keyword)) \
		)

#define MAX_OPERATOR_LENGTH 3

#define COLOR_RED   "\x1B[31m"
#define COLOR_GREEN   "\x1B[32m"
#define COLOR_YELLOW   "\x1B[33m"
#define COLOR_BLUE   "\x1B[34m"
#define COLOR_MAGENTA   "\x1B[35m"
#define COLOR_CYAN   "\x1B[36m"
#define COLOR_WHITE   "\x1B[37m"
#define COLOR_RESET "\x1B[0m"

#endif // MACROS_HPP