#include <iostream>
#include <fstream>
#include <vector>

int64_t SizeOfFile(std::fstream& file);

int64_t SizeOfFile(const std::string& file_name);

void CodeFile(const std::string& file_name, uint32_t m);

void DecodeFile(const std::string& file_name, uint32_t m);

void FixFile(const std::string& file_name, uint32_t m);