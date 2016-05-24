
#include <GarnetD.h>

#include <Windows.h>

#if defined(_WIN32)
/**
 *
 */
void  makeDir(const std::string& path)
{
    CreateDirectory(path.c_str(), NULL);
}

/**
 *
 */
bool  existsDir(const std::string& path)
{
    DWORD ret;
    return (ret = GetFileAttributes(path.c_str())) != 0xFFFFFFFF && (ret & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/**
 *
 */
bool  existsFile(const std::string& path)
{
    DWORD ret;
    return (ret = GetFileAttributes(path.c_str())) != 0xFFFFFFFF && (ret & FILE_ATTRIBUTE_FILE) == 0;
}

#else

/**
 *
 */
void  makeDir(const std::string& path)
{
    mkdir(path.c_str(), 0775);
}

/**
 *
 */
bool  existsDir(const std::string& path)
{
    mkdir(path.c_str(), 0775);
}

/**
 *
 */
bool  existsFile(const std::string& path)
{
    mkdir(path.c_str(), 0775);
}
#endif
