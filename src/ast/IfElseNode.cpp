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

#include "IfElseNode.h"

namespace Seserot {
    llvm::Value *AST::IfElseNode::codeGen(llvm::IRBuilder<> &irBuilder, llvm::LLVMContext &context) {
        llvm::Value *cond = condition->codeGen(irBuilder, context);
        if (!cond) {
            return nullptr;
        }
        // compare condition with 0 to get a boolean value
        cond = irBuilder.CreateICmpNE(cond, llvm::ConstantInt::get(context, llvm::APInt(1, 0, true)), "ifcond");
        llvm::Function *func = irBuilder.GetInsertBlock()->getParent();
        llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(context, "then", func);
        llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(context, "else");
        llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(context, "ifcont");
        irBuilder.CreateCondBr(cond, thenBlock, elseBlock);

        irBuilder.SetInsertPoint(thenBlock);
        llvm::Value *thenVal = this->thenBlock->codeGen(irBuilder, context);
        if (!thenVal) {
            return nullptr;
        }
        irBuilder.CreateBr(mergeBlock);
        thenBlock = irBuilder.GetInsertBlock(); // then中可以还有块，现在把then设置为最新的块，这样merge中的phi才能正确引用


        func->getBasicBlockList().push_back(elseBlock);
        irBuilder.SetInsertPoint(elseBlock);
        llvm::Value *elseVal = this->elseBlock->codeGen(irBuilder, context);
        if (!elseVal) {
            return nullptr;
        }
        irBuilder.CreateBr(mergeBlock);
        elseBlock = irBuilder.GetInsertBlock();
        func->getBasicBlockList().push_back(mergeBlock);
        irBuilder.SetInsertPoint(mergeBlock);
        llvm::PHINode *phiNode = irBuilder.CreatePHI(llvm::Type::getInt32Ty(context), 2, "iftmp");
        phiNode->addIncoming(thenVal, thenBlock);
        phiNode->addIncoming(elseVal, elseBlock);
        return phiNode;
    }
}  // namespace Seserot