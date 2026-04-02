//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_HELPER_H
#define VIEWER_HELPER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <uuid.h>


namespace BiBuild::Helper {


    inline std::string read_file(const char* filename) {

        std::string file_contents;
        std::ifstream file(filename);

        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();      // Зчитуємо буфер файлу в потік
            file_contents = buffer.str(); // Перетворюємо потік у рядок
            file.close();

        } else {
            std::cerr << "Error: couldn't open the file " << filename << std::endl;
            return file_contents;
        }

        std::cout << file_contents << std::endl;

        return file_contents;
    }

    inline uuids::uuid genUUID() {
        std::mt19937 engine{std::random_device{}()};
        uuids::uuid_random_generator gen{engine};
        uuids::uuid const id = gen();
        return id;
    }
}


#endif //VIEWER_HELPER_H
