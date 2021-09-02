#include <string>
#include <ctime>
#include <filesystem>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
SYSTEMTIME win32GetFileTime(std::string path);
#endif
std::time_t getFileTime(std::filesystem::path path);