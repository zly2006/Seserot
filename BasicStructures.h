//
// Created by 赵李言 on 2022/8/9.
//

#ifndef SESEROT_GEN0_BASIC_STRUCTURES_H
#define SESEROT_GEN0_BASIC_STRUCTURES_H
#include <string>
#include <utility>
#include <vector>

namespace Seserot {
    struct SourcePosition {
        std::string file;
        SourcePosition(int _line, int _column) : line(_line), column(_column) {}

        SourcePosition() {
            line = 0;
            column = 0;
        }

        [[nodiscard]] int compare(const SourcePosition& another) const {
            if (line > another.line) return 1;
            else if (line < another.line) return -1;
            else if (column > another.column) return 1;
            else if (column < another.column) return -1;
            else return 0;
        }

        bool operator<(const SourcePosition& another) const {
            return compare(another) < 0;
        }
        bool operator>(const SourcePosition& another) const {
            return compare(another) > 0;
        }
        bool operator==(const SourcePosition& another) const {
            return compare(another) == 0;
        }
        bool operator<=(const SourcePosition& another) const {
            return compare(another) <= 0;
        }
        bool operator>=(const SourcePosition& another) const {
            return compare(another) >= 0;
        }
        bool operator!=(const SourcePosition& another) const {
            return compare(another) != 0;
        }

        size_t line, column;
    };

    struct Scope {
        Scope(Scope *father, SourcePosition start, SourcePosition stop) : father(father), start(start), stop(stop) {}

        bool inside(Scope* father) {
            Scope* cur = this;
            while (cur->father != nullptr && cur->father != father) cur = cur->father;
            return cur->father == father;
        }

        Scope& newChildren(SourcePosition start, SourcePosition stop) {
            auto* p = new Scope(this, start, stop);
            children.push_back(p);
            return *p;
        }

        Scope *father = nullptr;
        std::vector<Scope*> children;
        char* tag = nullptr;
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
