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

#include "UnaryOperatorNode.h"

namespace Seserot::AST {
    llvm::Value *UnaryOperatorNode::codeGen(llvm::IRBuilder<> &irBuilder, llvm::LLVMContext &context) {
        //irBuilder.CreateBinOp(llvm::Instruction::BinaryOps::Add);
        irBuilder.CreateUnOp(llvm::Instruction::UnaryOps::UnaryOpsBegin, child->codeGen(irBuilder, context));
        return nullptr;
    }

    ASTNode::Actions UnaryOperatorNode::getAction() {
        return action;
    }
}