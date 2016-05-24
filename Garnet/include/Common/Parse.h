/*********************************************************
 *  Garnet
 *  Author: Munehiro Doi.
 *********************************************************/
#ifndef PARSE__H__
#define PARSE__H__

#include <string>
#include <vector>

#include <stdio.h>

typedef std::vector<std::string>  strings;

const int PARSE_SPACE   = 0x00000001; // separate by space
const int PARSE_TAB     = 0x00000002; // separate by tab
const int PARSE_COMMA   = 0x00000004; // separate by comma
const int PARSE_NEWLINE = 0x00000008; // separate by \n
const int PARSE_SQUOTE  = 0x00000100; // don't separate inside single quotation
const int PARSE_DQUOTE  = 0x00000200; // don't separate inside double quotation
const int PARSE_DEFAULT = 0x0000030f;

inline
strings  parse(const std::string& text, int mode = PARSE_DEFAULT)
{
    std::string  delim;
    if ( mode & PARSE_SPACE   ) delim += ' ';
    if ( mode & PARSE_TAB     ) delim += '\t';
    if ( mode & PARSE_COMMA   ) delim += ',';
    if ( mode & PARSE_NEWLINE ) delim += '\n';
    if ( mode & PARSE_NEWLINE ) delim += '\r';
    //printf("delim=[%s]\n", delim.c_str());
    
    
    strings ret;
    
    std::string::size_type i = 0;
    do
    {
        bool quoted = false;
        std::string token;
        std::string::size_type st, ed, del;
        
        st = i;
        ed = i;

        // Quotation
        if ( (text[i] == '\'' && (mode & PARSE_SQUOTE)) || (text[i] == '"' && (mode & PARSE_DQUOTE)) )
        {
            st = i + 1;
            ed = text.find_first_of(text[i], st);
            quoted = true;
        }

        // Find Delimiter
        del = text.find_first_of(delim, ed);
        
        // Cut the token
        if ( !quoted ) ed = del;

        if ( ed != std::string::npos )
        {
            ret.push_back(text.substr(st, ed - st));
        }
        else
        {
            ret.push_back(text.substr(st));
        }
        //printf("st=%u, ed=%u, del=%u\n", st, ed, del);
        
        if ( mode & PARSE_NEWLINE ) del = text.find_first_not_of("\n\r", del);
        if ( mode & PARSE_COMMA ) del = text.find_first_not_of(",", del);

		// Skip successive white space.
        i = text.find_first_not_of(" \t\r", del);
    }
    while ( i < text.size() );
    
    return ret;
}

inline
strings to_strings(int argc, const char* argv[])
{
	strings ret;
	for (int i = 0; i < argc; i++) {
		ret.push_back(argv[i]);
	}
	return ret;
}

#ifdef PARSE__TEST_ENABLED__
#include <iostream>

int main()
{
    const int   testType[] =
    {
        PARSE_DEFAULT,
        PARSE_SPACE,
        PARSE_TAB,
        PARSE_COMMA,
        PARSE_SPACE | PARSE_SQUOTE,
        PARSE_SPACE | PARSE_DQUOTE,
        PARSE_SPACE | PARSE_SQUOTE | PARSE_DQUOTE,
        0
    };
    const char* testAnswer[] =
    {
        "|a|b|c|d|e|f|g|h|i j\tk,l\n|n|o|p|q\"|",
        "|a|b\tc,d\ne|f\tg,h\n'i|j\tk,l\n'\"m|n\to,p\nq\"|",
        "|a b|c,d\ne f|g,h\n'i j|k,l\n'\"m n|o,p\nq\"|",
        "",
        "",
        "",
        "",
        "",
    };
    const char* testText = "a b\tc,d\ne f\tg,h\n'i j\tk,l\n'\"m n\to,p\nq\"";
    const int*   it;
    const char** jt;
    for (it = testType, jt = testAnswer; *it != 0; it++, jt++)
    {
        std::cout << "expected=" << *jt << "\n";
        
        strings tokens = parse(testText, *it);
        std::cout << "result  =";
        for (size_t i = 0; i< tokens.size(); i++)
        {
            std::cout << "|" << tokens[i];
        }
        std::cout << "|\n";
        std::cout << std::endl;
    }
    
    return 0;
}
#endif//PARSE__TEST_ENABLED__

#endif//PARSE__H__


