#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <regex>
#include <stdio.h>

#include "token.hpp"

using std::string;

class Lexer
{
private:
	string src;
	size_t size;
	size_t i;
public:
	Lexer(string src);
	Token next();
private:

};

#endif