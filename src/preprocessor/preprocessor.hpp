#ifndef PREPROCESSOR_PREPROCESSOR_HPP
#define PREPROCESSOR_PREPROCESSOR_HPP

#include <string>

using std::string;

void strip_comments(string& og);

void preprocess(string& src)
{
	// Strip comments
	strip_comments(src);

	// TODO: add support for other preprocess things such macros and stuff, basically everything that starts with '#' in c
}

void strip_comments(string& src)
{
	string result = "";
	result.reserve(src.size());

	bool s_cmt = false;
	bool m_cmt = false;
 
	for (int i=0; i<src.size(); i++)
	{
		if (s_cmt == true && src[i] == '\n')
			s_cmt = false;
		
		else if (m_cmt == true && src[i] == '*' && src[i+1] == '/')
			m_cmt = false,  i++;

		else if (s_cmt || m_cmt)
			continue;
 
		else if (src[i] == '/' && src[i+1] == '/')
			s_cmt = true, i++;
		else if (src[i] == '/' && src[i+1] == '*')
			m_cmt = true,  i++;
 
		else  result += src[i];
	}

	result.shrink_to_fit();
	src = result;
}

#endif // PREPROCESSOR_PREPROCESSOR_HPP