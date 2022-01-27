#ifndef MACROS_HPP
#define MACROS_HPP

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define USAGE_HELPER "Usage: \"dig filepath [options]\"\n\texample: \"dig main.dg -o main.exe\"\n\
Options:\n\
\t-o <file>\t\t\t\t\tSet output file to <file>, the default will be the file name but with .exe extension\n\
"

#define ERROR_WE_DONT_KNOW "Something went horribly wrong, probably a bug in the compiler itself, sorry"
#define UNEXPECTED_CHARACTER(ch) "We reached an unexpected character: '%c'", ch
#define ERROR_TWO_FLOAT_DOTS "Float number has two or more dots, it should only have one"
#define ERROR_NO_MATCHING_QUOTE "No closing double quotes for this pair, add it somewhere"
#define ERROR_CHAR_TOO_LONG "Single quotes are meant for single character literals, more were given"

#define IS_HEXA_DIGIT(c) ( (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') )

#endif // MACROS_HPP