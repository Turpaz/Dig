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

	void error(const char* format, ...);
	void error(size_t at, const char* format, ...);
};

#endif // LEXER_LEXER_HPP