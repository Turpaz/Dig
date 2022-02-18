#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "transpiler/transpiler.hpp"
#include "preprocessor/preprocessor.hpp"

#include <string>
#include <fstream>
#include <streambuf>
#include <string.h>
#include <getopt.h>

using std::string;

string getfile(string name);
inline bool does_file_exist(string path);
inline void get_options(int argc, char** argv, int &opts, string& path, string& _o);

int main(int argc, char** argv)
{
	int cmd_options = cmd_args::None;
	string path = "";
	string output_file;
	string src;
	bool dont_compile = false;

	//  -----=+*/ PARSE COMMAND LINE ARGUMENTS \*+=-----
	// Make sure that there are command line arguments
	if (argc < 2) { fprintf(stderr, "No input file\n\n" USAGE_HELPER); exit(-1); }
	// Get the path, unless the first argument is a flag, then we'll stop comipilng before getting the file
	if (argv[1][0] != '-')
		path.assign(argv[1]);
	else
		dont_compile = true;
	// Get the options
	get_options(argc, argv, cmd_options, path, output_file);

	//  -----=+*/ IMPLEMENT THE COMMAND LINE FLAGS THAT WE CAN \*+=-----
	// If the -h flag is set, print the help page and continue
	if (cmd_options & cmd_args::_h) { printf("%s", USAGE_HELPER); }
	// If the -v flag is set, print the version continue
	if (cmd_options & cmd_args::_v) { printf("%s", "Dig version: " DIG_VERSION "\n"); }

	// If the don't compile variable is true - exit here with exit code 0
	if (dont_compile)
		exit(0);

	//  -----=+*/ GET THE FILE AND SET THE OUTPUT FILE NEEDED \*+=-----
	// Check if file exist
	if (!does_file_exist(path)) { fprintf(stderr, "Input file \"%s\" doesn't exist\n\n", path.c_str()); exit(-1); }
	// set output_file if needed
	if (!(cmd_options & cmd_args::_o))
		output_file = path.substr(0, path.find_last_of('.')) + ".exe";
	// Get the file
	src = getfile(path);

	//  -----=+*/ BEGINNING OF THE COMPILATION PROCESS! \*+=-----

	// Preprocess
	preprocess(src); // FIXME: Might be problematic, may interfere with the error position. we might have to skip comments in the lexer

	// If the -E flag is set, print the code and exit with exit code 0
	if (cmd_options & cmd_args::_E) { printf("%s", src.c_str()); exit(0); }

	Lexer lexer(src, path);
	Parser parser(&lexer);
	//Transpiler transpiler;

	TokenNode* root = new TokenNode();
	Token tok;
	while (lexer.get_token(tok))
	{
		root->add_child(tok);
	}
	root->add_child(Token());

	// root->print();

	parser.parse(*root);

	// parser.print();

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

inline void get_options(int argc, char** argv, int &opts, string& path, string& _o)
{
	int opt;

	while ((opt = getopt(argc, argv, ":" CMD_OPTIONS_STRING)) != -1)
	{
		switch(opt)
		{
			case 'o':
				opts |= cmd_args::_o;
				_o.assign(optarg);
				break;
			case 'h':
				opts |= cmd_args::_h;
				break;
			case 'v':
				opts |= cmd_args::_v;
				break;
			case 'E':
				opts |= cmd_args::_E;
				break;
			case ':':
				printf("Option -%c requires a value.\n", optopt);
				exit(-1);
			case '?':
				printf("Unknown option: -%c.\n", optopt);
				exit(-1);
			default:
				fprintf(stderr, ERROR_WE_DONT_KNOW);
				exit(-1);
		}
	}
}