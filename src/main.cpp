#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "transpiler/transpiler.hpp"
#include "preprocessor/preprocessor.hpp"

#include <string>
#include <fstream>
#include <streambuf>
#include <string.h>

using std::string;

string getfile(string name);
inline bool does_file_exist(string path);

int main(int argc, char** argv)
{
	unsigned cmd_options = static_cast<unsigned>(CmdArgs::None);
	string path;
	string output_file;
	string src;

	// Check path and options
	if (argc < 2) { fprintf(stderr, "No input file\n\n" USAGE_HELPER); exit(-1); }
	for (size_t i = 2; i < argc; i++)
	{
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) 	cmd_options |= CmdArgs::_h;
		if (!strcmp(argv[i], "-E")) 								cmd_options |= CmdArgs::_E;
		if (!strcmp(argv[i], "-o")) { output_file = argv[i+1];		cmd_options |= CmdArgs::_o; }
	}
	

	// Get file path
	path = argv[1];
	if (!does_file_exist(path)) { fprintf(stderr, "Input file doesn't exist\n\n" USAGE_HELPER); exit(-1); }
	// set output_file
	if (cmd_options & CmdArgs::_o)
		output_file = "";
	else
		output_file = path.substr(0, path.find_last_of('.')) + ".exe"; // FIXME: might not be correct

	// TODO: Bother with implementing options

	// Get the file
	src = getfile(path);

	// Preprocess
	preprocess(src);

	// If the -E flag is set, print the code and exit with exit code 0
	if (cmd_options & CmdArgs::_E) { printf("%s", src.c_str()); exit(0); }

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