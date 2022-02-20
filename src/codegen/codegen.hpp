#ifndef TRANSPILER_TRANSPILER_HPP
#define TRANSPILER_TRANSPILER_HPP

#include "../parser/tree.hpp"
#include "../parser/parser.hpp"
#include <string>

using std::string;

class Codegen
{
private:
	Parser* parser;
public:
	string prolog;
	string data;
	string bss;
public:
	Codegen(Parser* parser);
	~Codegen() {}

	void generate();

	string tostr() const;

	inline void print() const
	{
		printf("%s", tostr().c_str());
	}
};

#endif // TRANSPILER_TRANSPILER_HPP