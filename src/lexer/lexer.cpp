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
		skip_comment();

		if ( isalpha(c()) || c() == '_') /* if c is in the alphabet or its a _ */
			return parse_alpha();
		if ( isdigit(c()) || (c() == '.' && isdigit(c(i+1))) ) /* if c is a digit or its a . but has a digit after it */
			return parse_digit();
		if ( c() == '\'' )
			return parse_digit();
		if ( c() == '"' )
			return parse_string();
		
		// TODO: add parse_operator() function
		// TODO: add more operators
		switch (c())
		{
		case '(':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::LPAREN, this->i);
		case ')':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::RPAREN, this->i);
		case '{':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::LBRACE, this->i);
		case '}':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::RBRACE, this->i);
		case '=':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::ASS, this->i);
		case ';':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::SEMI, this->i);
		case '+':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::PLUS, this->i);
		case '-':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::MINUS, this->i);
		case '*':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::MUL, this->i);
		case '/':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::DIV, this->i);
		case '>':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::GT, this->i);
		case '<':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::LT, this->i);
		case ',':
			increment(); return Token(toktype::KEYWORD, (unsigned)operators::COMMA, this->i);
		case '\0':
			break;
		default:
			error(UNEXPECTED_CHARACTER(c()));
		}
	}

	return Token();
}

Token Lexer::parse_alpha()
{
	size_t alphaBegin = i;
	std::string v = "";

	do
	{
		v += c();
		increment();
	} while (isalpha(c()) || c() == '_' || isdigit(c()));
	
	// if it's a keyword (or a variable type)
	if (getKeywordOrVartypeFromString(v))
		return Token(toktype::KEYWORD, getKeywordOrVartypeFromString(v), alphaBegin);
	
	return Token(toktype::IDENTIFIER, v, alphaBegin);
}

Token Lexer::parse_digit()
{
	size_t digitBegin = i;
	string v = "";
	long double number = 0;
	char mode = 0; // 0 = decimal, 1 = hexadecimal, 2 = binary, 3 = octal

	if (c() == '\'')
	{
		increment();

		while (c() != '\'')
		{
			v += c();
			increment();
		}
		
		if (v[0] == '\\' && v.size() == 2)
		{
			switch (v[1])
			{
			case 'n':
				v = "\n"; break;
			case 't':
				v = "\t"; break;
			case 'v':
				v = "\v"; break;
			case 'b':
				v = "\b"; break;
			case 'r':
				v = "\r"; break;
			case 'f':
				v = "\f"; break;
			case 'a':
				v = "\a"; break;
			case '\\':
				v = "\\"; break;
			case '?':
				v = "\?"; break;
			case '\'':
				v = "\'"; break;
			case '\"':
				v = "\""; break;
			case '0':
				v = "\0"; break;
			default:
				break;
			}
		}
		if (v.size() > 1) error(ERROR_CHAR_TOO_LONG);

		number = v[0];
		return Token(toktype::NUM, number, digitBegin);
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
			if (has_dot) 	number = std::stold(v);
			else 			number = std::stoull(v);}
		else if (mode == 1)
			number = std::stoull(v, NULL, 16);
		else if (mode == 2)
			number = std::stoull(v, NULL, 2);
		else if (mode == 3)
			number = std::stoull(v, NULL, 8);
		else
			error(ERROR_WE_DONT_KNOW);

		return Token(toktype::NUM, number, digitBegin);
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
	
	return Token(toktype::NUM, number, digitBegin);
}

Token Lexer::parse_string()
{
	string v = "";
	size_t firstQuotePos = i;

	increment();

	while (c() != '"' || (c() == '"' && c(i-1) == '\\'))
	{
		if (c() == '\0')
			error(firstQuotePos, ERROR_NO_MATCHING_QUOTE);
		
		v += c();
		increment();
	}

	increment();
	
	/* TODO: account for special characters, currently "ab\ncd" will take the \n part literally ("ab\\ncd").
	It should understand it and translate is to:
	"ab
	cd"
	*/
	return Token(toktype::STRING, v, firstQuotePos);
}

void Lexer::increment()
{
	if (i < size && c() != '\0')
	{
		i++;
	}
}

void Lexer::skip_blank()
{
	while (c() == '\r' || c() == '\n' || c() == ' ' || c() == '\t')
	{
		increment();
	}
}

void Lexer::skip_comment()
{
	// TODO: account to a situation of a whitespace then a comment then more whitespace (i.e "  /* cmnt*/  // cmnt")
	if (c() == '/' && c(i+1) == '/')
	{
		while (c() != '\n' && c() != '\0')
		{
			increment();
		}
		increment();increment();
	}

	if (c() == '/' && c(i+1) == '*')
	{
		while ( !(c() == '*' && c(i+1) == '/') )
		{
			increment();
		}
		increment();increment();
	}
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