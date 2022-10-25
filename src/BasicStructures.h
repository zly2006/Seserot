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

#ifndef SESEROT_GEN0_BASIC_STRUCTURES_H
#define SESEROT_GEN0_BASIC_STRUCTURES_H

#include <string>
#include <utility>
#include <vector>
#include <cassert>
#include <optional>
#include <cmath>
#include <algorithm>
#include <variant>
#include <memory>

#include "utils/sum_string.h"

#undef LLVM_ENABLE_ABI_BREAKING_CHECKS

#define HERE sum("At file ", __FILE__, " line ", __LINE__, __func__)

#ifdef TEXT
#undef TEXT
#endif
#define TEXT(x) #x

int main(int, char **, char **);

namespace Seserot {
    using int64 = long long;
    using uint64 = unsigned long long;
    using int32 = int;
    using uint32 = unsigned int;
    using int16 = short;
    using uint16 = unsigned short;
    using int8 = char;
    using uint8 = unsigned char;

    struct SourcePosition {
        std::string file;

        SourcePosition(int _line, int _column) : line(_line), column(_column) {}

        SourcePosition() {
            line = 0;
            column = 0;
        }

        [[nodiscard]] int compare(const SourcePosition &another) const {
            if (line > another.line) return 1;
            else if (line < another.line) return -1;
            else if (column > another.column) return 1;
            else if (column < another.column) return -1;
            else return 0;
        }

        bool operator<(const SourcePosition &another) const {
            return compare(another) < 0;
        }

        bool operator>(const SourcePosition &another) const {
            return compare(another) > 0;
        }

        bool operator==(const SourcePosition &another) const {
            return compare(another) == 0;
        }

        bool operator<=(const SourcePosition &another) const {
            return compare(another) <= 0;
        }

        bool operator>=(const SourcePosition &another) const {
            return compare(another) >= 0;
        }

        bool operator!=(const SourcePosition &another) const {
            return compare(another) != 0;
        }

        size_t line, column;
    };

    struct Scope {
        Scope(Scope *father, SourcePosition start, SourcePosition stop) : father(father), start(std::move(start)),
                                                                          stop(std::move(stop)) {}

        bool inside(Scope *scope) {
            assert(scope != nullptr);
            Scope *cur = this;
            while (cur->father != nullptr && cur->father != scope) cur = cur->father;
            return cur->father == scope;
        }

        Scope *newChildren(SourcePosition _start, SourcePosition _stop) {
            auto *p = new Scope(this, std::move(_start), std::move(_stop));
            p->id = children.size();
            children.push_back(p);
            return p;
        }

        [[nodiscard]] std::string getSignature() const { // NOLINT(misc-no-recursion)
            assert(father != this);
            if (father != nullptr) {
                return father->getSignature() + "$" + std::to_string(id);
            }
            else {
                return std::to_string(id);
            }
        };

        Scope *father = nullptr;
        std::vector<Scope *> children;
        size_t id;
        SourcePosition start, stop;


        ~Scope() {
            for (auto &item: children) {
                delete item;
            }
        }
    };

    struct FileScope : Scope {
        FileScope() : Scope(nullptr, SourcePosition(0, 0), SourcePosition(0, 0)) {}
    };

    struct Token {
        enum Type {
            Name,
            Literal,
            Number,
            Operator,
            NewLine,
            Character
        };
        SourcePosition start, stop;
        Type type;
        std::string content;
        enum ParsedType {
            Ready,
            Statement,
            Parsed,
        };
        ParsedType parsed = Ready;
    };
}
#endif //SESEROT_GEN0_BASIC_STRUCTURES_H
