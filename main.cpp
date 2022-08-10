#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "BasicStructures.h"
#include "Lexer.h"

int main() {
    std::string ("a")[0];
    std::string () + 'a';
    std::cout << "Hello, World!" << std::endl;
    Seserot::ErrorTable errorTable;
    std::ifstream fin("helloworld.se");
    std::string file;
    while (fin) {
        std::string a;
        std::getline(fin, a);
        file += a + '\n';
    }
    std::cout<<file;
    Seserot::Lexer lexer(errorTable, file);
    lexer.parse();
    for (auto&item: lexer.tokens) {
        std::cout<<item.content<<"\n";
    }
    return 0;
}
