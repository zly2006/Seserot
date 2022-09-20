/*********************************************************************
Seserot - My toy compiler
Copyright (C) 2022  zly2006

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*********************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Lexer.h"
#include "Parser.h"

#include <filesystem>

std::optional<int> build(std::filesystem::path path) {
    std::filesystem::directory_iterator iterator(path);
    if (!iterator->exists()) {
        return {};
    }
    for (auto &item : iterator) {
        std::filesystem::current_path();
    }
    return 0;
}

int main(int _argc, char **_argv) {
    std::vector<std::string> args;
    for (int i = 1; i < _argc; ++i) {
        args.emplace_back(_argv[i]);
    }
    std::cout << "Seserot gen0 "<< sizeof(long)*8<<"bit\n";
    Seserot::ErrorTable errorTable;
    std::ifstream fin("../helloworld.se");
    std::string file;
    while (fin) {
        std::string a;
        std::getline(fin, a);
        file += a + '\n';
    }
    std::cout<<file;
    Seserot::Lexer lexer(errorTable, file);
    lexer.parse();
    Seserot::Parser parser(lexer.tokens, errorTable);
    parser.parse();
    std::cout << "namespaces\n";
    for (auto&item: parser.namespaces) {
        std::cout<<item.first<<"\n";
    }
    std::cout << "classes\n";
    for (auto&item: parser.classes) {
        std::cout<<item.second->toString()<<"\n";
    }
    std::cout << "methods\n";
    for (auto&item: parser.methods) {
        std::cout<<item.first<<"\n";
    }
    return 0;
}
