#include "hammimgcode.h"

int FindCodedWordLength(uint32_t m) {
    int j = 2;
    int k = 1;
    while (j < k + 8 * m + 1) {
        j *= 2;
        k += 1;
    }
    return 8 * m + k;
}

int64_t SizeOfFile(std::fstream &file) {
    file.seekg(0, std::ios::end);
    int64_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    return size;
}

int64_t SizeOfFile(const std::string& file_name) {
    std::fstream file(file_name, std::ios::in | std::ios::binary);
    int64_t size = SizeOfFile(file);
    file.close();
    return size;
}

bool PowerOfTwo(int Value) {
    int InitValue = 1;
    while (InitValue < Value) {
        InitValue *= 2;
    }

    if (InitValue == Value) {
        return true;
    }

    return false;
}

std::string ByteToStringOfBits(char c) {
    std::string s;
    bool b;
    for (int i = 0; i < 8; ++i) {
        b = bool((1 << i) & c);
        if (b) {
            s.insert(0, "1");
        } else {
            s.insert(0, "0");
        }
    }

    return s;
}

char StringToByte(const std::string& s) {
    char c = 0;
    bool sign;
    for (int i = 0; i < 8; ++i) {
        sign = bool(s[i] - '0');

        if (sign) {
            c |= (1 << (7 - i));
        } else {
            c &= ~(1 << (7 - i));
        }
    }

    return c;
}

std::string AddHammingBits(const std::string& block) {
    std::string s = "00";
    int j = 3;
    int i = 0;

    while (i < block.length()) {
        if (!PowerOfTwo(j + i)) {
            s += block[i];
            i++;
        } else {
            s += '0';
            j++;
        }
    }

    return s;
}

std::string CodeHammingBits(const std::string& block) {
    uint32_t len = block.length();
    std::string s = block;

    for (int i = 1; i < len + 1; i *= 2) {
        int sum = 0;
        for (int j = i - 1; j < len; j += 2 * i) {
            for (int k = 0; k < i and j + k < len; ++k) {
                sum += s[j + k] - 48;
            }
        }

        s[i - 1] = char(sum % 2 + 48);
    }

    return s;
}

std::string FixBlock(const std::string& block) {
    uint32_t len = block.length();
    std::string s = block;
    uint32_t index_of_broken_bit = 0;

    for (int i = 1; i < len + 1; i *= 2) {
        int sum = 0;
        for (int j = i - 1; j < len; j += 2 * i) {
            for (int k = 0; k < i and j + k < len; ++k) {
                sum += s[j + k] - '0';
            }
        }

        if (s[i - 1] != char((sum + s[i - 1]) % 2 + '0')) {
            index_of_broken_bit += i;
        }
    }

    if (index_of_broken_bit != 0) {
        s[index_of_broken_bit - 1] = char((s[index_of_broken_bit - 1] + 1) % 2 + '0');
    }

    return s;
}

void FixFile(const std::string& file_name, uint32_t m) {
    std::fstream file_to_fix(file_name, std::ios::binary | std::ios::in);
    std::string tmp_file_name = file_name + "_fixed";
    std::fstream fixed_file(tmp_file_name, std::ios::binary | std::ios::out);

    char byte[1];
    std::string s;
    std::string buff;
    std::string block;

    int len = FindCodedWordLength(m);
    uint32_t file_size = SizeOfFile(file_to_fix);
    for (int i = 0; i < file_size; ++i) {
        file_to_fix.read(byte, 1);
        s += ByteToStringOfBits(byte[0]);

        if (s.length() >= len) {
            block = s.substr(0, len);
            block = FixBlock(block);
            s = s.substr(len);
            buff += block;
        }

        while (buff.length() >= 8) {
            byte[0] = StringToByte(buff.substr(0, 8));
            fixed_file.write(byte, 1);
            buff = buff.substr(8);
        }
    }

    //работаем с хвостом:
    block = FixBlock(s);
    buff += block;

    while (buff.length() > 0) {
        byte[0] = StringToByte(buff.substr(0, 8));
        fixed_file.write(byte, 1);
        buff = buff.substr(8);
    }

    file_to_fix.close();
    fixed_file.close();

    std::remove(file_name.c_str());
    std::rename(tmp_file_name.c_str(), file_name.c_str());
}

std::string DecodeBlock(const std::string& block) {
    uint32_t len = block.length();
    std::string s;

    for (int i = 0; i < len; ++i) {
        if (!PowerOfTwo(i + 1)) {
            s += block[i];
        }
    }

    return s;
}

void DecodeFile(const std::string& file_name, uint32_t m) {
    FixFile(file_name, m);

    std::fstream file_to_decode(file_name, std::ios::binary | std::ios::in);
    std::string tmp_file_name = file_name + "_decoded";
    std::fstream decoded_file(tmp_file_name, std::ios::binary | std::ios::out);

    int len = FindCodedWordLength(m);
    uint32_t file_size = SizeOfFile(file_to_decode);


    char byte[1];
    std::string s;
    std::string buff;
    std::string block;

    for (int i = 0; i < file_size; ++i) {
        file_to_decode.read(byte, 1);
        s += ByteToStringOfBits(byte[0]);

        if (s.length() >= len) {
            block = s.substr(0, len);
            block = DecodeBlock(block);
            s = s.substr(len);
        }

        //тут гарантировано длина блока будет кратна 8
        while (block.length() > 0) {
            byte[0] = StringToByte(block.substr(0, 8));
            decoded_file.write(byte, 1);
            block = block.substr(8);
        }
    }

    //работаем с хвостом:

    block = DecodeBlock(s);

    while (block.length() >= 8) {
        byte[0] = StringToByte(block.substr(0, 8));
        decoded_file.write(byte, 1);
        block = block.substr(8);
    }

    file_to_decode.close();
    decoded_file.close();

    std::remove(file_name.c_str());
    std::rename(tmp_file_name.c_str(), file_name.c_str());
}

void CodeFile(const std::string& file_name, uint32_t m) {
    std::fstream file_to_code(file_name, std::ios::binary | std::ios::in);
    std::string tmp_file_name = file_name + "_coded";
    std::fstream coded_file(tmp_file_name, std::ios::binary | std::ios::out);

    char byte[1];
    std::string s;
    std::string buff;
    std::string block;

    int64_t file_size = SizeOfFile(file_name);
    for (int j = 0; j < file_size; ++j) {
        file_to_code.read(byte, 1);
        s += ByteToStringOfBits(byte[0]);

        if (s.length() == m * 8) {
            block = AddHammingBits(s);
            block = CodeHammingBits(block);
            s = "";
            buff += block;
        }

        while (buff.length() >= 8) {
            byte[0] = StringToByte(buff.substr(0, 8));
            coded_file.write(byte, 1);
            buff = buff.substr(8);
        }
    }
    //работаем с оставшимся хвостом бит:

    if (!s.empty()) {
        block = AddHammingBits(s);
        block = CodeHammingBits(block);
        buff += block;
    }


    uint32_t count_zero = (8 - buff.length() % 8);
    for (int i = 0; i < count_zero; ++i) {
        buff += '0';
    }

    while (buff.length() >= 8) {
        byte[0] = StringToByte(buff.substr(0, 8));
        coded_file.write(byte, 1);
        buff = buff.substr(8);
    }


    file_to_code.close();
    coded_file.close();

    std::remove(file_name.c_str());
    std::rename(tmp_file_name.c_str(), file_name.c_str());
}