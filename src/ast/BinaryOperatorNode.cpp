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

#include "BinaryOperatorNode.h"

namespace Seserot::AST {
    llvm::Value *BinaryOperatorNode::codeGen(llvm::IRBuilder<> &irBuilder, llvm::LLVMContext &context) {
        llvm::Value *leftValue = left->codeGen(irBuilder, context);
        llvm::Value *rightValue = right->codeGen(irBuilder, context);
        switch (action) {
            case Actions::Add:
                return irBuilder.CreateAdd(leftValue, rightValue);
            case Actions::Subtract:
                return irBuilder.CreateSub(leftValue, rightValue);
            case Actions::Multiple:
                return irBuilder.CreateMul(leftValue, rightValue);
            case Actions::Divide:
                return irBuilder.CreateSDiv(leftValue, rightValue);
            case Actions::Mod:
                return irBuilder.CreateSRem(leftValue, rightValue);
            default:
                return nullptr;
        }
    }

    ASTNode::Actions BinaryOperatorNode::getAction() {
        return action;
    }
}  // namespace Seserot::AST