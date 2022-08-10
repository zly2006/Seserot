//
// Created by 赵李言 on 2022/8/10.
//

#include "CompilerError.h"
#include <iostream>
namespace Seserot {
    std::string CompilerError::print() {
        return std::string("    E:") + "[" + category + "]" + message;
    }
} // Seserot