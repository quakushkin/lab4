#include "hamarc.h"
#include "parsing.h"

int main(int argc, char *argv[]) {

    Arguments arguments = Parsing(argc, argv);

    std::vector<std::string> list = arguments.file_list;
    std::string arch_name = arguments.archive_name;
    uint32_t code_word_len = arguments.word_length;

//    std::vector<std::string> list;
//    std::string arch_name = "123.haf";
//    arguments.option = 1;
//    uint32_t code_word_len;

    if (arguments.option == 0) {
        CreateArchive(arch_name, list, code_word_len);
    } else if (arguments.option == 5) {
        Merge(arch_name, list);
    } else if (arguments.option == 1) {
        ListOfFiles(arch_name);
    } else if (arguments.option == 2) {
        if (list.empty()) {
            ExtractAll(arch_name);
        } else {
            for (int i = 0; i < list.size(); ++i) {
                ExtractFile(arch_name, list[i]);
            }
        }
    } else if (arguments.option == 3) {
        for (int i = 0; i < list.size(); ++i) {
            Append(list[i], arch_name, code_word_len);
        }
    } else if (arguments.option == 4) {
        for (int i = 0; i < list.size(); ++i) {
            Delete(arch_name, list[i]);
        }
    }

    return 0;
}