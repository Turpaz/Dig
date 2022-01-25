#include "lexer.hpp"

Lexer::Lexer(string src)
{
	this->src = src;
	this->size = src.size();
	this->i = 0;
}