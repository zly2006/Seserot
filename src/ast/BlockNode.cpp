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

#include "BlockNode.h"

#include <utility>

namespace Seserot::AST {
    // 实际上，这并没有创建一个block，因为这里没有label，只要依次对子语句codegen就行了
    llvm::Value *BlockNode::codeGen(llvm::IRBuilder<> &irBuilder, llvm::LLVMContext &context) {
        llvm::Function *function = irBuilder.GetInsertBlock()->getParent();
        // The value of the last expression in the block is the return value of the block.
        llvm::Value *value;
        for (const auto &item: statements) {
            value = item->codeGen(irBuilder, context);
            if (!value) {
                return nullptr;
            }
        }
        return value;
    }

    BlockNode::BlockNode(std::string signature): signature(std::move(signature)) {}
}  // namespace Seserot::AST

