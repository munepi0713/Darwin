#ifndef INLINE_SPRINTF__H__
#define INLINE_SPRINTF__H__

#include <cstdio>
#include <cstdarg>
#include <string>
#include <cwchar>

#pragma warning(disable : 4996)

template <typename CharType>
static
std::basic_string<CharType>  inline_sprintf(const CharType* format, ...)
{
	CharType buf[1] = {0};
    return buf;
}

template <>
inline
std::string  inline_sprintf(const char* format, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    return buf;
}

template <>
inline
std::wstring  inline_sprintf(const wchar_t* format, ...)
{
    wchar_t buf[1024];
    va_list args;
    va_start(args, format);
    vswprintf(buf, sizeof(buf), format, args);
    va_end(args);
    return buf;
}

#endif//INLINE_SPRINTF__H__
