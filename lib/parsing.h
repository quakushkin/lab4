#include <cstdint>
#include <iostream>
#include <string>
#include <vector>


struct Arguments {
    uint8_t option = -1;
    std::string archive_name;
    uint32_t word_length;
    std::vector<std::string> file_list;
};

Arguments Parsing(int argc, char *argv[]);