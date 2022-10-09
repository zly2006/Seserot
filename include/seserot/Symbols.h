//
// Created by 赵李言 on 2022/10/6.
//

#ifndef SESEROT_GEN0_SYMBOLS_H
#define SESEROT_GEN0_SYMBOLS_H

#include <string>
typedef struct {
    int id;
    std::string name;
    std::string type;
    std::string value;
    int scope;
    int line;
    int column;
} Symbol;
extern "C" {
Symbol *search(std::string name);
void insert(std::string name, const char *type, const char *value, int scope, int line, int column);
void print();
};
#endif //SESEROT_GEN0_SYMBOLS_H
