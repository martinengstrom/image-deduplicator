#!/bin/bash
# https://stackoverflow.com/questions/18138635/mingw-exe-requires-a-few-gcc-dlls-regardless-of-the-code
# https://stackoverflow.com/questions/9618815/i-dont-want-console-to-appear-when-i-run-c-program
rm my.res
windres my.rc -O coff -o my.res
g++ -Os -ffunction-sections -fdata-sections -Wl,--gc-sections \
    -std=c++20 -static -static-libgcc -static-libstdc++ \
    -mwindows \
    main.cpp \
    md5.cpp \
    ui/folderSelect.cpp \
    ui/prompt.cpp \
    hash.cpp \
    modtime.cpp \
    moveFile.cpp \
    my.res
# g++ --std=c++17 -static -static-libgcc -static-libstdc++ hashfile.cpp md5.cpp
