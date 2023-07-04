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
#include <mutex>
#include <thread>
#include <chrono>
#include <queue>
#include "ui/folderSelect.h"
#include "hash.h"
#include "modtime.h"
#include "moveFile.h"
#include "ui/prompt.h"

#ifdef __linux__
    #include <gtk/gtk.h>
#endif

auto findDuplicates(std::map<std::string, std::vector<std::tuple<std::filesystem::path, std::time_t>>> files) {
    std::vector<std::vector<std::tuple<std::filesystem::path, std::time_t>>> result;
    for (const auto& [hash, path] : files) {
        if (path.size() > 1) {
            result.push_back(path);
        }
    }
    return result;
}

void thread_get_hash(
        int id, 
        std::mutex *queueMutex, 
        std::mutex *resultMutex,
        std::queue<std::filesystem::path> *paths,
        std::map<std::string, std::vector<std::tuple<std::filesystem::path, std::time_t>>> *result) {

    std::vector<std::string> extensions = { ".jpg", ".jpeg", ".png" };
    while (true) {
        queueMutex->lock();
        if (paths->empty()) {
            queueMutex->unlock();
            return;
        }

        std::filesystem::path p = paths->front();
    paths->pop();
        queueMutex->unlock();

        if (std::find(extensions.begin(), extensions.end(), p.extension()) != extensions.end()) {
            std::string hashval = hash(p.string());

        resultMutex->lock();
        bool at_end = result->find(hashval) == result->end();
        resultMutex->unlock();

        if (at_end) {
                std::vector<std::tuple<std::filesystem::path, std::time_t>> v;
                v.push_back(std::make_tuple(p, getFileTime(p)));
                resultMutex->lock();
                result->insert_or_assign(hashval, v);
                resultMutex->unlock();
        } else {
                resultMutex->lock();
        result->at(hashval).push_back(std::make_tuple(p, getFileTime(p)));
                resultMutex->unlock();
        }
        }
    }
}

void thread_counter(int total, std::mutex *queueMutex, std::queue<std::filesystem::path> *paths) {
    using namespace std::chrono_literals;
    bool keep_going = true;
    std::this_thread::sleep_for(100ms);
    int total_sleep = 0;
    while (keep_going) {
        if (total_sleep < 10000) {
            total_sleep += 100;
            std::this_thread::sleep_for(100ms);
            continue;
        }
        std::this_thread::sleep_for(5000ms);
        int amount = 0;
        queueMutex->lock();
        if (paths->empty()) keep_going = false;
        else {
            amount = paths->size();
        }
        queueMutex->unlock();
        std::cout << "[" << (total - amount) << "/" << total << "]" << std::endl;
        std::this_thread::sleep_for(2000ms);
    }
}

auto threaded_find_duplicates(std::string path, int thread_count) {
    auto *result = new std::map<std::string, std::vector<std::tuple<std::filesystem::path, std::time_t>>>();

    auto *paths = new std::queue<std::filesystem::path>();
    for (const auto& dirEntry : std::filesystem::directory_iterator(path)) {
        paths->push(dirEntry.path());
    }

    int total = paths->size();
    std::mutex *queueMutex = new std::mutex();
    std::mutex *resultMutex = new std::mutex();
    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(thread_get_hash, i, queueMutex, resultMutex, paths, result);
    }
    //threads.emplace_back(thread_counter, total, queueMutex, paths);

    for (auto& thread : threads) {
        thread.join();
    }

    auto foo = findDuplicates(*result);
    delete queueMutex;
    delete resultMutex;
    delete paths;
    delete result;
    return foo;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

int main(int argc, char* argv[]) {
    #ifdef __linux__
        gtk_init(&argc, &argv);
    #endif
    bool list = false;
    bool verbose = false;
    bool invert = false;
    if (cmdOptionExists(argv, argv+argc, "-l")) {
        list = true;
    }

    if (cmdOptionExists(argv, argv+argc, "-v")) {
        verbose = true;
    }

    if (cmdOptionExists(argv, argv+argc, "-i")) {
        invert = true;
    }

    std::string path = BrowseFolder(argv[argc - 1], "Select source directory");
    if (path.empty()) return 1;

    auto dupes = threaded_find_duplicates(path, 10);
    
    // Go through each vector, find which file is the oldest and remove it from the list
    for (auto& fileSet : dupes) {
        const std::tuple<std::filesystem::path, std::time_t> *first = fileSet.data();
        for (const auto& tuple : fileSet) {
        bool newer = std::get<1>(*first) > std::get<1>(tuple);
        bool select = invert ? !newer : newer;

        if (select && (std::get<1>(*first) == std::get<1>(tuple))) {
                std::string f_filename = std::get<0>(*first);
        std::string t_filename = std::get<0>(tuple);

        if (t_filename.size () < f_filename.size()) first = &tuple;    
        } else if (select) {
                first = &tuple;
            }
        }

        if (verbose) std::cout << "Found original: " << std::get<0>(*first).string() << std::endl;
        fileSet.erase(std::remove(fileSet.begin(), fileSet.end(), *first), fileSet.end());
    }

    std::cout << "Found " << dupes.size() << " duplicate images" << std::endl;
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
                std::cerr << "Failed moving " << p.string() << std::endl;
            }
        }
    }

    return 0;
}
