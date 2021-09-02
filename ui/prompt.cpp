#include "prompt.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
bool win32Prompt(std::string title, std::string body) {
    int result = MessageBox(NULL, body.c_str(), title.c_str(), 
        MB_OKCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1);

    // Return values are:
    // 3 Abort pressed
    // 2 Cancel pressed
    // 11 Continue pressed
    // 5 Ignore pressed
    // 7 No pressed
    // 1 OK Pressed
    // 4 Retry pressed
    // 10 Try again pressed
    // 6 Yes pressed

    return result == 1;
}
#endif

bool prompt(std::string title, std::string body) {
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        return win32Prompt(title, body);
    #else
        #error UI needs GTK implementation
    #endif
}