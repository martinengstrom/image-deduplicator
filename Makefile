UNAME := $(shell uname)
CC = g++
PROGRAM_NAME = test
OBJ = main.cpp md5.cpp ui/folderSelect.cpp ui/prompt.cpp hash.cpp modtime.cpp moveFile.cpp
STD = 

ifeq ($(UNAME), Linux)
FLAGS = 
default : $(OBJ)
	$(CC) -std=c++20 $(shell pkg-config --cflags gtk+-3.0) $(OBJ) -o $(PROGRAM_NAME) $(shell pkg-config --libs gtk+-3.0)
endif

SUB_UNAME := $(findstring MINGW32, $(UNAME))

ifeq ($(SUB_UNAME), MINGW32)
default-w32 : $(OBJ)
	rm my.res
	windres my.rc -O coff -o my.res
	$(CC) -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=c++20 -static -static-libgcc -static-libstdc++ -mwindows $(OBJ) my.res -o $(PROGRAM_NAME)
endif
