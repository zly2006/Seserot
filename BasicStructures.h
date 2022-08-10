//
// Created by 赵李言 on 2022/8/9.
//

#ifndef SESEROT_GEN0_BASIC_STRUCTURES_H
#define SESEROT_GEN0_BASIC_STRUCTURES_H
#include <string>
namespace Seserot {
    struct SourcePosition {
        SourcePosition(int _line, int _column) : line(_line), column(_column) {}

        SourcePosition() {
            line = 0;
            column = 0;
        }

        size_t line, column;
    };

    struct Scope {
        Scope(Scope *father, SourcePosition start, SourcePosition stop) : father(father), start(start), stop(stop) {}

        Scope *father;
        SourcePosition start, stop;
    };

    struct FileScope : Scope {
        FileScope() : Scope(nullptr, SourcePosition(0, 0), SourcePosition(0, 0)) {}
    };

    struct Symbol {
        enum Type {
            Class,
            GenericClass,
            Method,
            Property,
            Value,
            Variable,
        };
        Scope scope;
        Type type;
    };

    struct Token {
        enum Type {
            Name,
            Literal,
            Number,
            Operator,
        };
        SourcePosition start, stop;
        Type type;
        std::string content;
    };
}
#endif //SESEROT_GEN0_BASIC_STRUCTURES_H
