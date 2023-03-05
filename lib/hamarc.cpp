#include "hamarc.h"
#include "hammimgcode.h"

const uint16_t SIZE_OF_CODED_HEADER = 418;

void ReadFileHeader(FileHeader& header, std::fstream& archive) {
    char byte[1];
    std::fstream tmp("q", std::ios::binary | std::ios::out);

    for (int i = 0; i < SIZE_OF_CODED_HEADER; ++i) {
        archive.read(byte, 1);
        tmp.write(byte, 1);
    }

    tmp.close();
    DecodeFile("q", 1);

    std::fstream decoded_tmp("q", std::ios::binary | std::ios::in);

    decoded_tmp.read((char *) &header.code_word_lenght, 4);
    decoded_tmp.read((char *) &header.coded_file_size, 8);
    decoded_tmp.read((char *) &header.real_file_size, 8);
    decoded_tmp.read((char *) &header.file_name_size, 2);
    decoded_tmp.read((char *) &header.file_name, 256);

    decoded_tmp.close();
    std::remove("q");
}

void WriteFileHeader(FileHeader& header, std::fstream& archive) {
    std::fstream header_file("a", std::fstream::out | std::ios::binary);

    header_file.write((char *) &header.code_word_lenght, 4);
    header_file.write((char *) &header.coded_file_size, 8);
    header_file.write((char *) &header.real_file_size, 8);
    header_file.write((char *) &header.file_name_size, 2);
    header_file.write((char *) &header.file_name, 256);

    header_file.close();
    CodeFile("a", 1);

    WriteFile("a", archive);
    std::remove("a");
}

void WriteFile(const std::string& file_name, std::fstream& archive) {
    int64_t file_size = SizeOfFile(file_name);
    std::fstream file(file_name, std::ios::in | std::ios::binary);

    char byte[1];
    for (uint64_t j = 0; j < file_size; ++j) {
        file.read(byte, 1);
        archive.write(byte, 1);
    }
}

void GenerateHeader(FileHeader& header, int64_t size, uint32_t code_word_lenght, int64_t coded_size,
                    const std::string &name) {
    header.real_file_size = size;
    header.code_word_lenght = code_word_lenght;
    header.coded_file_size = coded_size;
    header.file_name_size = name.length();

    for (int i = 0; i < name.length(); ++i) {
        header.file_name[i] = name[i];
    }
}

void Append(const std::string& file_path, std::fstream& archive, uint32_t code_word_lenght) {
    int64_t size = SizeOfFile(file_path);
    std::string file_name = file_path.substr(file_path.rfind('\\') + 1);

    CodeFile(file_path, code_word_lenght);
    int64_t coded_size = SizeOfFile(file_path);

    FileHeader header;
    GenerateHeader(header, size, code_word_lenght, coded_size, file_name);
    WriteFileHeader(header, archive);
    WriteFile(file_path, archive);

    DecodeFile(file_path, code_word_lenght);
}

void Append(const std::string& file_path, const std::string& archive_path, uint32_t code_word_lenght) {
    std::fstream archive(archive_path, std::ios::binary | std::ios::app);

    Append(file_path, archive, code_word_lenght);

    archive.close();
}

void CreateArchive(const std::string& archive_full_name, const std::vector<std::string>& file_pathes,
                   uint32_t code_word_lenght) {
    std::fstream archive(archive_full_name, std::ios::binary | std::ios::out);

    for (int i = 0; i < file_pathes.size(); ++i) {
        Append(file_pathes[i], archive, code_word_lenght);
    }

    archive.close();
}

std::string GetNameOfFile(char *file_name, uint16_t file_name_size) {
    std::string s;
    for (int i = 0; i < file_name_size; ++i) {
        s += file_name[i];
    }
    return s;
}

void ListOfFiles(const std::string& archive_full_name) {
    std::fstream archive(archive_full_name, std::fstream::in | std::ios::binary);
    uint64_t archive_size = SizeOfFile(archive);

    int64_t i = 0;
    while (i < archive_size) {
        FileHeader header;
        ReadFileHeader(header, archive);

        std::cout << GetNameOfFile(header.file_name, header.file_name_size) << "\n";
        archive.seekg(header.coded_file_size, std::ios::cur);
        i += SIZE_OF_CODED_HEADER + header.coded_file_size;
    }

    archive.close();
}

void Extract(std::fstream& archive, FileHeader& header) {
    std::fstream file(header.file_name, std::fstream::out | std::ios::binary);

    char byte[1];
    for (uint64_t j = 0; j < header.coded_file_size; ++j) {
        archive.read(byte, 1);
        file.write(byte, 1);
    }

    file.close();

    DecodeFile(header.file_name, header.code_word_lenght);
}

void ExtractAll(const std::string& archive_full_name) {
    std::fstream archive(archive_full_name, std::fstream::in | std::ios::binary);
    uint64_t archive_size = SizeOfFile(archive);

    uint64_t i = 0;

    while (i < archive_size) {
        FileHeader header;
        ReadFileHeader(header, archive);
        Extract(archive, header);
        i += SIZE_OF_CODED_HEADER + header.coded_file_size;
    }

    archive.close();
}

void ExtractFile(const std::string& archive_full_name, const std::string& file_name) {
    std::fstream archive(archive_full_name, std::fstream::in | std::ios::binary);
    uint64_t archive_size = SizeOfFile(archive);

    int64_t i = 0;
    while (i < archive_size) {
        FileHeader header;
        ReadFileHeader(header, archive);

        std::string current_name = GetNameOfFile(header.file_name, header.file_name_size);
        if (current_name == file_name) {
            Extract(archive, header);
        } else {
            archive.seekg(header.coded_file_size, std::ios::cur);
        }

        i += SIZE_OF_CODED_HEADER + header.coded_file_size;
    }

    archive.close();
}

void Delete(const std::string& archive_full_name, const std::string& file_name) {
    std::fstream old_archive(archive_full_name, std::fstream::in | std::ios::binary);

    std::string new_archive_name = archive_full_name + "tmp";
    std::fstream new_archive(new_archive_name, std::fstream::app | std::ios::binary);

    char byte[1];
    int64_t archive_size = SizeOfFile(old_archive);
    int64_t i = 0;

    while (i < archive_size) {
        FileHeader header;
        ReadFileHeader(header, old_archive);

        std::string current_name = GetNameOfFile(header.file_name, header.file_name_size);
        if (current_name == file_name) {
            old_archive.seekg(header.coded_file_size, std::ios::cur);
        } else {
            WriteFileHeader(header, new_archive);
            for (int64_t j = 0; j < header.coded_file_size; ++j) {
                old_archive.read(byte, 1);
                new_archive.write(byte, 1);
            }
        }

        i += SIZE_OF_CODED_HEADER + header.coded_file_size;
    }
    old_archive.close();
    new_archive.close();
    std::remove(archive_full_name.c_str());
    std::rename(new_archive_name.c_str(), archive_full_name.c_str());
}

void Merge(const std::string& archive_full_name, const std::vector<std::string>& archive_pathes) {
    std::fstream new_archive(archive_full_name, std::fstream::out | std::ios::binary);
    uint64_t size;
    char byte[1];
    for (int i = 0; i < archive_pathes.size(); ++i) {
        std::fstream archive(archive_pathes[i], std::fstream::in | std::ios::binary);
        size = SizeOfFile(archive);

        for (int j = 0; j < size; ++j) {
            archive.read(byte, 1);
            new_archive.write(byte, 1);
        }
    }
}
