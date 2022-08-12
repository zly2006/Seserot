//
// Created by 赵李言 on 2022/8/10.
//

#include "CompilerError.h"
#include <iostream>
#include <utility>
namespace Seserot {
    std::string CompilerError::print() {
        return std::string("    E:") + "[" + category + "]" + message;
    }

    CompilerError::CompilerError(const SourcePosition &where, size_t code, std::string message, std::string category)
            : where(where), code(code), message(std::move(message)), category(std::move(category)) {}
} // Seserot