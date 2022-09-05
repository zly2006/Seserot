#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Lexer.h"
#include "Parser.h"

int main() {
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
    parser.scan();
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
