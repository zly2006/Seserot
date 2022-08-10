//
// Created by 赵李言 on 2022/8/10.
//

#ifndef SESEROT_GEN0_ERROR_TABLE_H
#define SESEROT_GEN0_ERROR_TABLE_H
#include "CompilerError.h"
#include "vector"
namespace Seserot {
    class ErrorTable {
    public:
        std::vector<CompilerError> errors{};
        void interrupt(std::string = "");
    };
}

#endif //SESEROT_GEN0_ERROR_TABLE_H
