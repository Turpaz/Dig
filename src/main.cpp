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
inline bool does_file_exist(string path);

int main(int argc, char** argv)
{
	string path;
	string output_file;
	string src;

	// Get path and options
	if (argc < 2) { fprintf(stderr, "No input file\n\n" USAGE_HELPER); exit(-1); }
	// TODO: Bother with getting options

	// Get file path and set output_file (if -o option is set this will change)
	path = argv[1];
	if (!does_file_exist(path)) { fprintf(stderr, "Input file doesn't exist\n\n" USAGE_HELPER); exit(-1); }
	output_file = path.substr(0, path.find_last_of('.')) + ".exe"; // FIXME: might not be correct

	// TODO: Bother with implementing options

	// Get the file
	src = getfile(path);

	// Preprocess
	preprocess(src);

	// Begging of actual compilation process
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

inline bool does_file_exist(string path)
{
	std::ifstream f(path.c_str());
	return f.good();
}