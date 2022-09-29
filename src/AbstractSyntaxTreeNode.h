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

#ifndef SESEROT_GEN0_ABSTRACT_SYNTAX_TREE_NODE_H
#define SESEROT_GEN0_ABSTRACT_SYNTAX_TREE_NODE_H
#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <llvm/CodeGen/ValueTypes.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include "ByteCodeWriter.h"
#include "Symbol.h"

namespace Seserot {

    class Parser;
    class AbstractSyntaxTreeNode {
    public:
        AbstractSyntaxTreeNode();

        AbstractSyntaxTreeNode(const AbstractSyntaxTreeNode &node);

        AbstractSyntaxTreeNode(AbstractSyntaxTreeNode &&node) noexcept;

        AbstractSyntaxTreeNode &operator=(const AbstractSyntaxTreeNode &rhs);

        ~AbstractSyntaxTreeNode();

        llvm::Value *CodeGen(Parser&);

        enum Actions {
            Add,
            Subtract,
            Multiple,
            Divide,
            Mod,
            Call,
            LogicAnd,
            LogicOr,
            LogicNot,
            BitAnd,
            BitOr,
            BitNot,
            BitXor,
            LeftShift,
            RightShift,
            CallVirtual,
            Assignment,
            LiteralByte,
            LiteralSByte,
            LiteralShort,
            LiteralUShort,
            LiteralInt,
            LiteralUInt,
            LiteralLong,
            LiteralULong,
            LiteralDouble,
            LiteralString,
            NewInstance,
            Error,
        };
        enum Tags {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
            None = 0,
#pragma clang diagnostic pop
            Completed = 1
        };
        Tags tag = Tags::None;
        Actions action = Error;
        std::vector<AbstractSyntaxTreeNode> children = {};

        size_t write(char *);

        void read(char *, size_t);

        // char*是用来保证new/delete的，所以new的时候必须用char
        char *data = nullptr;
        size_t dataLength = 0;
        ClassSymbol *typeInferred = nullptr;
    };

} // Seserot

#endif //SESEROT_GEN0_ABSTRACT_SYNTAX_TREE_NODE_H
