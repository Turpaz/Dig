#ifndef MACROS_HPP
#define MACROS_HPP

#define DIG_VERSION "1.0.0"

#define USAGE_HELPER "Usage: dig input_file_path [options]\n\texamples:\n\
\t\tdig main.dg -o main.exe\n\
\t\tdig main.dg\n\
\t\tdig -h\n\
Options:\n\
\t-o <file>\t\t\tSet output file to <file>, the default will be the file name but with .exe extension.\n\
\t-h\t\t\t\tShows this message.\n\
\t-v\t\t\t\tShows the version of the compiler.\n\
\n\
\t-E\t\t\tPreprocess only.\n\
"

 // Max 32 command line arguments
 #define CMD_OPTIONS_STRING "o:hve" /* all the options */
class cmd_args
{
public:
    static const int None	= 0;
    static const int _o		= (1 << 0);
    static const int _h		= (1 << 1);
    static const int _v		= (1 << 2);
    static const int _E		= (1 << 3);
};

#define ERROR_WE_DONT_KNOW "Something went horribly wrong, probably a bug in the compiler itself, sorry"
#define UNEXPECTED_CHARACTER(ch) "We reached an unexpected character: '%c'", ch
#define ERROR_TWO_FLOAT_DOTS "Float number has two or more dots, it should only have one"
#define ERROR_NO_MATCHING_QUOTE "No closing double quotes for this pair, add it somewhere"
#define ERROR_CHAR_TOO_LONG "Single quotes are meant for single character literals, more were given"

#define IS_HEXA_DIGIT(c) ( (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') )

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#endif // MACROS_HPP