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
        switch (action) {
            case ASTNode::LogicNot:
                if (child->inferredType == BuiltinSymbols::Boolean) {
                    return irBuilder.CreateNot(child->codeGen(irBuilder, context));
                } else {
                    throw std::runtime_error("LogicNot only accepts bool");
                }
                break;
            case ASTNode::BitNot:
                if (child->inferredType->afterOrEqual(BuiltinSymbols::Number)) {
                    return irBuilder.CreateNot(child->codeGen(irBuilder, context));
                } else {
                    throw std::runtime_error("BitNot only accepts number");
                }
                break;
            default:
                break;
        }
        // irBuilder.CreateBinOp(llvm::Instruction::BinaryOps::Add);
        irBuilder.CreateUnOp(llvm::Instruction::UnaryOps::UnaryOpsBegin, child->codeGen(irBuilder, context));

        return nullptr;
    }

    ASTNode::Actions UnaryOperatorNode::getAction() {
        return action;
    }

    UnaryOperatorNode::UnaryOperatorNode(ASTNode::Actions action, std::unique_ptr<ASTNode> child):
            action(action), child(std::move(child)) {}
}  // namespace Seserot::AST