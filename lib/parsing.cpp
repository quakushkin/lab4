#include "parsing.h"

Arguments Parsing(int argc, char *argv[]) {
    Arguments arguments;

    for (int i = 1; i < argc; ++i) {
        std::string tmp = argv[i];
        //будем считать, что за один ввод в консоль можно запустить только одну функцию
        //иначе возможно слишком много вариантов последовательностей функций и их аргументов
        //совсем не ясно, какие команды парсер должен уметь обрабатывать, а какие нет :(
        if (tmp[0] == '-') {
            if (tmp == "-f") {
                if (i < argc - 1) {
                    arguments.archive_name = argv[i + 1];
                    i++;
                } else {
                    std::cerr << "something wrong...";
                }
            } else if (tmp.find("--file=") != -1) {
                arguments.archive_name = tmp.substr(tmp.find("--file=") + 7);
            } else if (tmp == "-c" or tmp == "--create") {
                arguments.option = 0;
            } else if (tmp == "-l" or tmp == "--list") {
                arguments.option = 1;
            } else if (tmp == "-x" or tmp == "--extract") {
                arguments.option = 2;
            } else if (tmp == "-a" or tmp == "--append") {
                arguments.option = 3;
            } else if (tmp == "-d" or tmp == "--delete") {
                arguments.option = 4;
            } else if (tmp == "-A" or tmp == "--concatenate") {
                arguments.option = 5;
            } else if (tmp == "-w" or tmp == "--word") {
                if (i < argc - 1) {
                    arguments.word_length = std::stoi(argv[i + 1]);
                    i++;
                } else {
                    std::cout << "something wrong...";
                }
            }
        } else {
            arguments.file_list.push_back(tmp);
        }

    }

    return arguments;
}