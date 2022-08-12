//
// Created by 赵李言 on 2022/8/10.
//

#ifndef SESEROT_GEN0_COMPILER_ERROR_H
#define SESEROT_GEN0_COMPILER_ERROR_H
#include "BasicStructures.h"
namespace Seserot {

    class CompilerError {
    public:
        SourcePosition where;
        size_t code;
        std::string message;
        std::string category;

        explicit CompilerError(const SourcePosition &where, size_t code, std::string message, std::string category = "");

        virtual std::string print();
    };

} // Seserot

#endif //SESEROT_GEN0_COMPILER_ERROR_H
