/*********************************************************************
Seserot - My toy compiler
Copyright (C) 2022  zly2006

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the i            mplied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*********************************************************************/

#ifndef SESEROT_AST_NODE_H
#define SESEROT_AST_NODE_H

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

namespace Seserot::AST {
    class CodeGenerator {
    public:
        llvm::LLVMContext *context;
        llvm::IRBuilder<> *irBuilder;

        CodeGenerator() {
            context = new llvm::LLVMContext;
            irBuilder = new llvm::IRBuilder<>(*context);
        }
    };

    class ASTNode {
    public:
        enum Actions {
            Unknown,
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

        virtual llvm::Value *codeGen(llvm::IRBuilder<> &irBuilder, llvm::LLVMContext &context) = 0;

        virtual ~ASTNode() = default;

        virtual Actions getAction() {
            return ASTNode::Unknown;
        }

        ASTNode() = default;
    };

} // Seserot

#endif
