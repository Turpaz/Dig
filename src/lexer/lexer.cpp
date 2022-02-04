#include "lexer.hpp"

Lexer::Lexer(string src, string path)
{
	this->src = src;
	this->path = path;
	this->size = src.size();
	this->i = 0;
}

Token Lexer::next()
{
	while (c() != '\0')
	{
		skip_blank();

		if ( isalpha(c()) || c() == '_') /* if c is in the alphabet or its a _ */
			return parse_alpha();
		if ( isdigit(c()) || (c() == '.' && isdigit(c(i+1))) ) /* if c is a digit or its a . but has a digit after it */
			return parse_digit();
		if ( c() == '\'' )
			return parse_digit();
		if ( c() == '"' )
			return parse_string();
		
		return parse_operator();
	}

	return Token();
}

Token Lexer::parse_alpha()
{
	size_t index = this->i;
	std::string v = "";

	do
	{
		v += c();
		increment();
	} while (isalpha(c()) || c() == '_' || isdigit(c()));
	
	// if it's a keyword (or a variable type)
	if (getKeywordOrVartypeFromString(v))
		return Token(toktype::KEYWORD, getKeywordOrVartypeFromString(v), index);
	
	return Token(toktype::IDENTIFIER, v, index);
}

Token Lexer::parse_digit()
{
	string v = "";
	size_t index = this->i;
	double number = 0;
	char mode = 0; // 0 = decimal, 1 = hexadecimal, 2 = binary, 3 = octal

	if (c() == '\'')
	{
		increment();

		while (c() != '\'' || (c() == '\'' && c(i-1) == '\\' && c(i-2) != '\\'))
		{
			v += c();
			increment();
		}
		
		v = account_special_characters(v);
		
		if (v.size() > 1) error(ERROR_CHAR_TOO_LONG);

		number = v[0];
		return Token(toktype::NUM, number, index);
	}

	if (c() != '.')
	{
		bool has_dot = false;

		// while the character is a digit, or a dot (getting taking care of in the loop),
		// OR if it's the second character with the first being 0, it can be x or b for
		// changing number mode to hexadecimal or binary, then - if the mode is hexadecimal
		// also accept characters between a-f or A-F with a macro
		while ( isdigit(c()) || c() == '.' || (v == "0" && (c() == 'x' || c() == 'b') || c() == 'o') \
		|| ( mode == 1 && IS_HEXA_DIGIT(c()) ) )
		{
			if (c() == '.')
			{
				if (has_dot == true)
					error(ERROR_TWO_FLOAT_DOTS);
				has_dot = true;
			}
			else
			{
				if (c() == 'x') { mode = 1; }
				else if (c() == 'b') { mode = 2; v = ""; }
				else if (c() == 'o') { mode = 3; v = ""; }
			}

			if (c() != 'b' && c() != 'o')
				v += c();
			increment();
		}

		if (c(i-1) == '.') /* if the last digit is a dot- add a zero (12. -> 12.0) */
			v += '0';

		if (mode == 0){
			if (has_dot) 	number = std::stod(v);
			else 			number = std::stoull(v);}
		else if (mode == 1)
			number = std::stoull(v, NULL, 16);
		else if (mode == 2)
			number = std::stoull(v, NULL, 2);
		else if (mode == 3)
			number = std::stoull(v, NULL, 8);
		else
			error(ERROR_WE_DONT_KNOW);

		return Token(toktype::NUM, number, index);
	}

	// else
	v += "0."; /* if the first char is . then add a 0 before it */
	increment();

	while (isdigit(c()))
	{
		v += c();
		increment();
	}

	number = std::stold(v);
	
	return Token(toktype::NUM, number, index);
}

Token Lexer::parse_string()
{
	string v = "";
	size_t index = this->i;
	
	increment();

	while (c() != '"' || (c() == '"' && c(i-1) == '\\') && c(i-2) != '\\')
	{
		if (c() == '\0')
			error(index, ERROR_NO_MATCHING_QUOTE);
		
		v += c();
		increment();
	}

	increment();
	
	v = account_special_characters(v);

	return Token(toktype::STRING, v, index);
}

Token Lexer::parse_operator()
{
	string v = "";
	size_t index = this->i;

	// Get the operator (or the operator plus some unnecessary characters)
	for (int i = 0; i < MAX_OPERATOR_LENGTH; i++)
	{
		if (CAN_BE_OPERATOR(c()))
		{
			v += c();
			increment();
		}else break;
	}

	// check if the operator is in the language, if so - return the needed token, otherwise- decrement and try with a shorter string again
	for (; v.size() > 0;)
	{
		#define KEYWORD(id, str)
		#define VARTYPE(id, str)
		#define OPERATOR(id, str) if (v == str) return Token(toktype::KEYWORD, static_cast<unsigned>(operators::id), index);
		#include "../grammar/language.inc"
		#undef KEYWORD
		#undef VARTYPE
		#undef OPERATOR

		v.pop_back();
		decrement();
	}

	error(UNEXPECTED_CHARACTER(c()));

	return Token();
}

void Lexer::increment()
{
	if (i < size && c() != '\0')
	{
		i++;
	}
}

void Lexer::decrement()
{
	if (i > 0)
	{
		i--;
	}
}

void Lexer::skip_blank()
{
	while (c() == '\r' || c() == '\n' || c() == ' ' || c() == '\t')
	{
		increment();
	}
}

string Lexer::account_special_characters(const string& og)
{
	string s = "";
	s.reserve(og.size());
	for (size_t i=0; i<og.size(); i++)
	{
		if (og[i] == '\\')
		{
			char single_char = '\\';
			switch (og[i+1])
			{
			case 'n':
				single_char = '\n'; break;
			case 't':
				single_char = '\t'; break;
			case 'v':
				single_char = '\v'; break;
			case 'b':
				single_char = '\b'; break;
			case 'r':
				single_char = '\r'; break;
			case 'f':
				single_char = '\f'; break;
			case 'a':
				single_char = '\a'; break;
			case '\\':
				single_char = '\\'; break;
			case '?':
				single_char = '\?'; break;
			case '\'':
				single_char = '\''; break;
			case '\"':
				single_char = '\"'; break;
			case '0':
				single_char = '\0'; break;
			default:
				break;
			}
			
			s += single_char;
			if (single_char != '\\')
				i++;
		}
		else
		{
			s += og[i];
		}
	}

	s.shrink_to_fit();
	return s;
}

void Lexer::error(const char* format, ...)
{
	size_t lineNumber = 1;
	size_t characterNumberInLine = 1;
	const char* j;
	const char* jj;
	// move j to i's character and count the '\n's in the way
	for (j = this->src.c_str(); j < this->src.c_str()+this->i && *j != '\0'; j++)
		if (*j == '\n')
			lineNumber++;
	// move jj to the begging of i's line
	for (jj = j; *jj != '\n' && jj > src; jj--) ;
	// calculate the column
	characterNumberInLine = j - jj;
	
	fprintf(stderr, "%s:%zu:%zu " ANSI_COLOR_RED "error: " ANSI_COLOR_RESET, path.c_str(), lineNumber, characterNumberInLine);

	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	fprintf(stderr, "\n");

	exit(-1);
}

void Lexer::error(size_t at, const char* format, ...)
{
	size_t lineNumber = 1;
	size_t characterNumberInLine = 1;
	const char* j;
	const char* jj;
	// move j to at's character and count the '\n's in the way
	for (j = this->src.c_str(); j < this->src.c_str()+at && *j != '\0'; j++)
		if (*j == '\n')
			lineNumber++;
	// move jj to the begging of at's line
	for (jj = j; *jj != '\n' && jj > src; jj--) ;
	// calculate the column
	characterNumberInLine = j - jj;
	
	fprintf(stderr, "%s:%zu:%zu " ANSI_COLOR_RED "error: " ANSI_COLOR_RESET, path.c_str(), lineNumber, characterNumberInLine);

	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	fprintf(stderr, "\n");

	exit(-1);
}