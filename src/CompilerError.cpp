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

#include "CompilerError.h"
#include <iostream>
#include <utility>

namespace Seserot {
    std::string CompilerError::toString() {
        char s[100];
        sprintf(s, "    E%zu[%s]:%s\n      @%s(line %zu, column %zu)", code, category.c_str(),
                message.c_str(), where.file.c_str(), where.line, where.column);
        return std::string(s); // NOLINT(modernize-return-braced-init-list)
    }

    CompilerError::CompilerError(SourcePosition where, size_t code, std::string message, std::string category)
            : where(std::move(where)), code(code), message(std::move(message)), category(std::move(category)) {}

    void CompilerError::print() {
        std::cout << toString() << "\n";
    }

    std::string CompilerWarning::toString() {
        char s[100];
        sprintf(s, "    W%zu[%s]:%s\n      @%s(line %zu, column %zu)", code, category.c_str(),
                message.c_str(), where.file.c_str(), where.line, where.column);
        return std::string(s); // NOLINT(modernize-return-braced-init-list)
    }

    CompilerWarning::CompilerWarning(SourcePosition where, size_t code, std::string message, std::string category)
            : CompilerError(std::move(where), code, std::move(message), std::move(category)) {

    }
} // Seserot