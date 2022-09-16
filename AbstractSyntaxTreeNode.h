//
// Created by 赵李言 on 2022/9/2.
//

#ifndef SESEROT_GEN0_ABSTRACTSYNTAXTREENODE_H
#define SESEROT_GEN0_ABSTRACTSYNTAXTREENODE_H
#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include "ByteCodeWriter.h"
#include "Symbol.h"

namespace Seserot {

    class AbstractSyntaxTreeNode {
    public:
        enum Actions {
            Add,
            Subtract,
            Multiple,
            Divide,
            Mod,
            Call,
            CallVirtual,
            Assignment,
            Error,
        };
        Actions action = Error;
        std::vector<AbstractSyntaxTreeNode> children = {};
        size_t write(char*);
        void read(char*, size_t);
        ClassSymbol* typeInferred;
    };

} // Seserot

#endif //SESEROT_GEN0_ABSTRACTSYNTAXTREENODE_H
