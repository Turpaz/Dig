#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "transpiler/transpiler.hpp"
#include "preprocessor/preprocessor.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>

using std::string;

string getfile(string name);

int main(int argc, char** argv)
{
	// TODO: Bother with command line arguments and make the program act as intended

	string path = "../test/test.dg"; // FIXME: look 2 lines up this is only temporary
	string src = getfile(path);

	// Preprocess
	preprocess(src);

	Lexer lexer(src, path);
	//Parser parser;
	//Transpiler transpiler;

	Token tok;
	while ((tok = lexer.next()).type != toktype::TOK_EOF)
	{
		tok.print();
	}
	

	return 0;
}

string getfile(string name)
{
	std::string str;
	std::ifstream f(name);

	f.seekg(0, std::ios::end);   
	str.reserve(f.tellg());
	f.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(f)),
				std::istreambuf_iterator<char>());
	
	return str;
}