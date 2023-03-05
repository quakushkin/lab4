#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct FileHeader {
    int64_t real_file_size;
    uint32_t code_word_lenght;
    int64_t coded_file_size;
    uint16_t file_name_size;
    char file_name[256]{};


    FileHeader() {
        code_word_lenght = 1;
        coded_file_size = 0;
        real_file_size = 0;
        file_name_size = 0;
    }
};

void CreateArchive(const std::string& archive_full_name, const std::vector<std::string>& file_pathes,
                   uint32_t code_word_lenght);

void Append(const std::string& file_path, std::fstream& archive, uint32_t code_word_lenght);

void Append(const std::string& file_path, const std::string& archive_path, uint32_t code_word_lenght);

void ReadFileHeader(FileHeader& header, std::fstream& archive);

void WriteFile(const std::string& file_name, std::fstream& archive);

void WriteFileHeader(FileHeader& header, std::fstream& archive);

void ListOfFiles(const std::string& archive_full_name);

void ExtractAll(const std::string& archive_full_name);

void ExtractFile(const std::string& archive_full_name, const std::string& file_name);

void Extract(std::fstream& archive, FileHeader& header);

void Delete(const std::string& archive_full_name, const std::string& file_name);

void Merge(const std::string &archive_full_name, const std::vector<std::string> &archive_pathes);

