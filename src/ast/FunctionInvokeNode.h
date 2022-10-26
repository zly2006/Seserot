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

#ifndef SESEROT_GEN0_FUNCTIONINVOKENODE_H
#define SESEROT_GEN0_FUNCTIONINVOKENODE_H

#include "../Symbol.h"
#include "ASTNode.h"
namespace Seserot::AST {

    /**
     * 表示函数调用<br/>
     * 不需要设置inferredType
     */
    class FunctionInvokeNode: public ASTNode {
    public:
        MethodSymbol *methodSymbol;

        std::string name;
        std::vector<std::unique_ptr<ASTNode>> args;

        explicit FunctionInvokeNode(MethodSymbol *methodSymbol);

        Actions getAction() override;

        llvm::Value *codeGen(llvm::IRBuilder<> &irBuilder, llvm::LLVMContext &context) override;
    };

}  // namespace Seserot::AST
#endif  // SESEROT_GEN0_FUNCTIONINVOKENODE_H
