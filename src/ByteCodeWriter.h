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

#ifndef SESEROT_GEN0_BYTECODE_WRITER_H
#define SESEROT_GEN0_BYTECODE_WRITER_H
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

#endif //SESEROT_GEN0_BYTECODE_WRITER_H
