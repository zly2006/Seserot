//
// Created by 赵李言 on 2022/8/10.
//

#include "CompilerError.h"
#include <iostream>
#include <utility>
namespace Seserot {
    std::string CompilerError::print() {
        char s[100];
        sprintf(s, "    E%zu[%s]:%s\n      @<?>(line %zu, column %zu)", code, category.c_str(),
                message.c_str(), where.line, where.column);
        return std::string(s); // NOLINT(modernize-return-braced-init-list)
    }

    CompilerError::CompilerError(const SourcePosition &where, size_t code, std::string message, std::string category)
            : where(where), code(code), message(std::move(message)), category(std::move(category)) {}
} // Seserot