//
// Created by 赵李言 on 2022/9/2.
//

#ifndef SESEROT_GEN0_BYTECODEWRITER_H
#define SESEROT_GEN0_BYTECODEWRITER_H
#include <utility>
#include <iostream>
namespace Seserot {
    class ByteCodeWriter {
    public:
        static int checksum(const char* binary, size_t len);
        static std::ostream& writeInt(std::ostream & ostream, int value);
        static int readInt(std::istream & istream);
    };
}

#endif //SESEROT_GEN0_BYTECODEWRITER_H
