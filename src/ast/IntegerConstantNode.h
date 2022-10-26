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

#ifndef SESEROT_INTEGER_CONSTANT_NODE_H
#define SESEROT_INTEGER_CONSTANT_NODE_H

#include "../BasicStructures.h"
#include "ASTNode.h"

namespace Seserot::AST {
    class IntegerConstantNode: public ASTNode {
    public:
        llvm::APInt v;

        explicit IntegerConstantNode(bool v);

        explicit IntegerConstantNode(int8 v);

        explicit IntegerConstantNode(uint8 v);

        explicit IntegerConstantNode(int16 v);

        explicit IntegerConstantNode(uint16 v);

        explicit IntegerConstantNode(int32 v);

        explicit IntegerConstantNode(uint32 v);

        explicit IntegerConstantNode(int64 v);

        explicit IntegerConstantNode(uint64 v);

        llvm::Value *codeGen(llvm::IRBuilder<> &irBuilder, llvm::LLVMContext &context) override;
    };
}  // namespace Seserot::AST
#endif