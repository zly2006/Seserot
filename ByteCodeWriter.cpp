//
// Created by 赵李言 on 2022/9/2.
//

#include "ByteCodeWriter.h"

int Seserot::ByteCodeWriter::checksum(const char *binary, size_t len) {
    int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += binary[i] * (int)((i * 35) % 28);
    }
    return sum;
}

std::ostream &Seserot::ByteCodeWriter::writeInt(std::ostream &ostream, int value) {
    ostream.write((char *) & value, 4);
    return ostream;
}

int Seserot::ByteCodeWriter::readInt(std::istream &istream) {
    int x;
    istream.read((char*)&x, 4);
    return x;
}
