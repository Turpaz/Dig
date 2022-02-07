#ifndef LEXER_LEXER_HPP
#define LEXER_LEXER_HPP

#include <string>
#include <regex>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "token.hpp"
#include "../macros.hpp"
#include "../grammar/grammar.hpp"

using std::string;

class Lexer
{
private:
	string src;
	string path;
	size_t size;
	size_t i;
public:
	Lexer(string src, string path);
	Token next();
	bool get_token(Token& tok) { tok = this->next(); return tok.type != toktype::TOK_EOF; }

	void error(const char* format, ...);
	void error(size_t at, const char* format, ...);
	void warning(const char* format, ...);
	void warning(size_t at, const char* format, ...);
private:
	Token parse_alpha();
	Token parse_digit();
	Token parse_string();
	Token parse_operator();
	void increment();
	void decrement();
	void skip_blank();
	
	string account_special_characters(const string& og);

	inline char c() const { return this->src[this->i]; }
	inline char c(size_t i) const { return this->src[i]; }
};

#endif // LEXER_LEXER_HPP