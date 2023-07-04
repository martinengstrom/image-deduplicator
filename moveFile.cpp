#include "moveFile.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#elif __linux__
#include <cstdio>
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
bool win32_moveFile(std::filesystem::path oldPath, std::filesystem::path newPath) {
    return MoveFile(oldPath.string().c_str(), newPath.string().c_str());
}
#elif __linux__
bool linux_moveFile(std::filesystem::path oldPath, std::filesystem::path newPath) {
    return !std::rename(oldPath.string().c_str(), newPath.string().c_str());
}
#endif

bool moveFile(std::filesystem::path oldPath, std::filesystem::path newPath) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    return win32_moveFile(oldPath, newPath);
#elif __linux__
    return linux_moveFile(oldPath, newPath);
#else
    return false;
#endif
}
