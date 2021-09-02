/*
    Martin "xealot" Engström
    xealot@gmail.com

    The purpose of this application is to take a source directory and 
    move any duplicate images to another directory.
*/

#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include "ui/folderSelect.h"
#include "hash.h"
#include "modtime.h"
#include "moveFile.h"
#include "ui/prompt.h"

auto iterateFS(std::string path) {
    std::vector<std::string> extensions = { ".jpg", ".jpeg", ".png" };
    std::map<std::string, std::vector<std::tuple<std::filesystem::path, std::time_t>>> result;

    for (const auto& dirEntry : std::filesystem::directory_iterator(path)) {
        std::filesystem::path p(dirEntry);
        if (std::find(extensions.begin(), extensions.end(), p.extension()) != extensions.end()) {
            std::string hashval = hash(dirEntry.path().string());
            if (result.find(hashval) == result.end()) {
                std::vector<std::tuple<std::filesystem::path, std::time_t>> v;
                v.push_back(std::make_tuple(dirEntry.path(), getFileTime(dirEntry.path())));
                result[hashval] = v;
            } else {
                result[hashval].push_back(std::make_tuple(dirEntry.path(), getFileTime(dirEntry.path())));
            }
        }
    }

    return result;
}

auto findDuplicates(std::map<std::string, std::vector<std::tuple<std::filesystem::path, std::time_t>>> files) {
    std::vector<std::vector<std::tuple<std::filesystem::path, std::time_t>>> result;
    for (const auto& [hash, path] : files) {
        if (path.size() > 1) {
            result.push_back(path);
        }
    }
    return result;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

int main(int argc, char* argv[]) {
    bool list = false;
    bool verbose = false;
    if (cmdOptionExists(argv, argv+argc, "-l")) {
        list = true;
    }

    if (cmdOptionExists(argv, argv+argc, "-v")) {
        verbose = true;
    }

    std::string path = BrowseFolder(argv[argc - 1], "Select source directory");
    auto dupes = findDuplicates(iterateFS(path));
    
    // Go through each vector, find which file is the oldest and remove it from the list
    for (auto& fileSet : dupes) {
        const std::tuple<std::filesystem::path, std::time_t> *first = fileSet.data();
        for (const auto& tuple : fileSet) {
            if (std::get<1>(*first) > std::get<1>(tuple)) {
                first = &tuple;
            }
        }

        if (verbose) std::cout << "Found original: " << std::get<0>(*first).string() << std::endl;
        fileSet.erase(std::remove(fileSet.begin(), fileSet.end(), *first), fileSet.end());
    }

    if (!prompt("Continue?", "Would you like to continue?"))
        return 1;

    std::string dest = BrowseFolder(argv[argc - 1], "Select destination directory");
    std::filesystem::path destPath(dest);

    for (const auto& fileSet : dupes) {
        for (const auto& [filepath, time] : fileSet) {
            std::filesystem::path p(filepath.filename());
            std::filesystem::path fsRes = destPath / p;

            std::cout << "Moving " << p.string() << std::endl;
            if (!moveFile(filepath, fsRes)) {
                std::cout << "Failed moving " << p.string() << std::endl;
            }
        }
    }

    return 0;
}