#include "modtime.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <Fileapi.h>
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
SYSTEMTIME win32GetFileTime(std::string path) {
    SYSTEMTIME lpLastWriteTime;
    SYSTEMTIME lpCreationTime;
    HANDLE fileHandle = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle) {
        FILETIME ftLastWriteTime;
        FILETIME ftCreationTime;
        if (GetFileTime(fileHandle, &ftCreationTime, NULL, &ftLastWriteTime)) {
            //FileTimeToSystemTime(&ftLastWriteTime, &lpLastWriteTime);
            FileTimeToSystemTime(&ftCreationTime, &lpCreationTime);
        }
    }
    return lpCreationTime;
}
#endif

std::time_t getFileTime(std::filesystem::path path) {
    auto ftime = std::filesystem::last_write_time(path);
    return std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(ftime));
}
