#include "moveFile.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
bool win32MoveFile(std::filesystem::path oldPath, std::filesystem::path newPath) {
    return MoveFile(oldPath.string().c_str(), newPath.string().c_str());
}
#endif

bool moveFile(std::filesystem::path oldPath, std::filesystem::path newPath) {
   #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    return win32MoveFile(oldPath, newPath);
   #endif
}