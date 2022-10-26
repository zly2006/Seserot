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

#include "IntegerConstantNode.h"

#include <llvm/IR/Constant.h>

namespace Seserot::AST {

    llvm::Value *IntegerConstantNode::codeGen(llvm::IRBuilder<> &irBuilder, llvm::LLVMContext &context) {
        return llvm::ConstantInt::get(context, v);
    }

    IntegerConstantNode::IntegerConstantNode(int8 v) {
        this->v = llvm::APInt(8, v, true);
    }

    IntegerConstantNode::IntegerConstantNode(uint8 v) {
        this->v = llvm::APInt(8, v, false);
    }

    IntegerConstantNode::IntegerConstantNode(int16 v) {
        this->v = llvm::APInt(16, v, true);
    }

    IntegerConstantNode::IntegerConstantNode(uint16 v) {
        this->v = llvm::APInt(16, v, false);
    }

    IntegerConstantNode::IntegerConstantNode(int32 v) {
        this->v = llvm::APInt(32, v, true);
    }

    IntegerConstantNode::IntegerConstantNode(uint32 v) {
        this->v = llvm::APInt(32, v, false);
    }

    IntegerConstantNode::IntegerConstantNode(int64 v) {
        this->v = llvm::APInt(64, v, true);
    }

    IntegerConstantNode::IntegerConstantNode(uint64 v) {
        this->v = llvm::APInt(64, v, false);
    }

    IntegerConstantNode::IntegerConstantNode(bool v) {
        this->v = llvm::APInt(1, v, false);
        inferredType = BuiltinSymbols::Boolean;
    }
}  // namespace Seserot::AST