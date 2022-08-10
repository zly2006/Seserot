//
// Created by 赵李言 on 2022/8/10.
//

#include "ErrorTable.h"
#include <iostream>
void Seserot::ErrorTable::interrupt(std::string where) {
    std::cout << where << "\n"
              << "Errors: \n";
    for (auto &item: errors) {
        std::cout << item.print() << "\n";
    }
    exit(0x50);
}
