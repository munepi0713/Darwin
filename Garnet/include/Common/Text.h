#ifndef COMMON__TEXT__H__
#define COMMON__TEXT__H__


#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <functional>
#include <cstdint>

namespace Text {

template <typename It>
std::string  join(It first, It last, const std::string& sep)
{
	bool isFirst = true;
	std::string ret;
	for (auto it = first; it != last; it++) {
		if ( isFirst ) {
			isFirst = false;
		}
		else {
			ret.append(sep);
		}
		ret.append(*it);
	}
	return ret;
}
	
static inline
uint32_t  countChar(const std::string& src, char c)
{
	uint32_t count = 0;
	for (auto it = src.cbegin(); it != src.cend(); it++) {
		if ( *it == c ) count++;
	}
	return count;
}

static inline
std::string  toLower(const std::string& src)
{
	std::string ret(src.size(), ' ');
	std::transform(src.begin(), src.end(), ret.begin(), std::ptr_fun(::tolower));
	return ret;
}

static inline
std::vector<std::string> split(const std::string& src, char delim)
{
	std::vector<std::string> ret;
	std::string::size_type st = 0, ed;
	for (;;) {
		ed = src.find_first_of(delim, st);
		if ( ed == std::string::npos ) {
			if ( st < src.length() ) {
				ret.push_back(src.substr(st));
			}
			break;
		}
		else {
			ret.push_back(src.substr(st, ed - st));
			st = ed + 1;
		}
	}
	return ret;
}

static inline
std::string trimLeft(const std::string& src)
{
	std::string::size_type pos;
	pos = src.find_first_not_of(" \t");
	return pos == std::string::npos ? "" : src.substr(pos);
}

static inline
std::string removeAfter(const std::string& src, const std::string& prefix)
{
	return src.substr(0, src.find(prefix));
}

static inline
bool equals(const std::string& a, const std::string& b)
{
	return a.compare(b) == 0;
}

static inline
bool equalsWithoutCase(const std::string& a, const std::string& b)
{
	std::string _a(a);
	std::string _b(b);
	std::transform(a.begin(), a.end(), _a.begin(), ::tolower);
	std::transform(b.begin(), b.end(), _b.begin(), ::tolower);
	return _a.compare(_b) == 0;
}

static inline
bool startsWith(const std::string& a, const std::string& b)
{
	return a.find(b) == 0;
}

static inline
bool endsWith(const std::string& a, const std::string& b)
{
	if ( a.length() < b.length() ) return false;
	return a.compare(a.length() - b.length(), b.length(), b) == 0;
}

/**
 *
 */
static inline
std::string  replaceAll(const std::string& src, const std::string& from, const std::string& to, unsigned int* numReplaced = 0)
{
	unsigned int count = 0;
	std::string ret = src;
	std::string::size_type pos = 0;
	while ( (pos = ret.find(from, pos)) != std::string::npos ) {
		ret = ret.replace(pos, from.length(), to);
		count++;
	}
	if ( numReplaced ) *numReplaced = count;
	return ret;
}

template <typename ContainerType, typename StringType>
int getIndex(const ContainerType& cont, const StringType& str)
{
	auto it = std::find(cont.cbegin(), cont.cend(), str);
	if (it != cont.cend()) {
		return (int)(it - cont.cbegin());
	}
	else {
		return -1;
	}
}



#if defined(_WIN32)

static inline
std::wstring  toWideChar(const std::string& src)
{
	int len = MultiByteToWideChar(CP_THREAD_ACP, MB_PRECOMPOSED, src.c_str(), (int)src.length(), 0, 0);
	if ( len == 0 ) {
        std::stringstream sout;
        sout << "Failed to MultiByteToWideChar (code=%u" << GetLastError() << ").";
		throw sout.str();
	}
	wchar_t* temp = new wchar_t[len + 1];
	MultiByteToWideChar(CP_THREAD_ACP, MB_PRECOMPOSED, src.c_str(), (int)src.length(), temp, len + 1);
	temp[len] = 0;
	std::wstring ret(temp);
	delete[] temp;
	return ret;
}

static inline
std::string  toMultiByte(const std::wstring& src)
{
	int len = WideCharToMultiByte(CP_THREAD_ACP, 0, src.c_str(), (int)src.length(), NULL, 0, NULL, NULL);
	if ( len == 0 ) {
        std::stringstream sout;
        sout << "Failed to WideCharToMultiByte (code=%u" << GetLastError() << ").";
		throw sout.str();
	}
	char* temp = new char[len + 1];
	WideCharToMultiByte(CP_THREAD_ACP, 0, src.c_str(), (int)src.length(), temp, len + 1, NULL, NULL);
	temp[len] = 0;
	std::string ret(temp);
	delete[] temp;
	return ret;
}

#else

#endif

}//end of namespace Text

#endif//COMMON__TEXT__H__
