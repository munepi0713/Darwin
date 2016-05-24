#ifndef COMMON__PATH__H__
#define COMMON__PATH__H__

#include <string>

namespace Path {


static
std::wstring resolvePath(const std::wstring& path, const std::wstring& base)
{
    // 1. If the path starts with '/', it will be returned as is.
    if ( path[0] == L'/' ) {
        return path;
    }
    
    // 2. Otherwise, base is connected in front of the path to make
    //    an absolute path.
	return (base.empty() ? L"./" : base + L'/') + path;
}

}//end of namespace Path

#endif//COMMON__PATH__H__
