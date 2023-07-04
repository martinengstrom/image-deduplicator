#include "hash.h"
#include <iostream>
#include <fstream>
#include "md5.h"

std::string hash(std::string filename) {
    MD5 md5;

    std::ifstream infile(filename, std::ios::binary);
    size_t buffer_size = 1024*1024;
    char buffer[buffer_size];

    while(infile) {
        infile.read(buffer, sizeof(buffer));
        md5.update(buffer, infile.gcount());
    }

    infile.close();
    md5.finalize();
    return md5.hexdigest();
}
