#ifndef TRANSPILER_TRANSPILER_HPP
#define TRANSPILER_TRANSPILER_HPP

#include "../parser/tree.hpp"
#include "../parser/parser.hpp"
#include <string>

using std::string;

class Codegen
{
private:
	string output_file;
	Parser* parser;
public:
	string prolog;
	string epilog;
	string data;
	string bss;
public:
	Codegen(Parser* parser, string output_file);
	~Codegen() {}

	void generate();

	string assemble_parts() const;

	inline void print() const
	{
		printf("%s", assemble_parts().c_str());
	}
};

#endif // TRANSPILER_TRANSPILER_HPP