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

#include "FunctionInvokeNode.h"

Seserot::AST::ASTNode::Actions Seserot::AST::FunctionInvokeNode::getAction() {
    return Actions::Call;
}

llvm::Value *Seserot::AST::FunctionInvokeNode::codeGen(llvm::IRBuilder<> &irBuilder, llvm::LLVMContext &context) {
    return nullptr;
}

Seserot::AST::FunctionInvokeNode::FunctionInvokeNode(Seserot::MethodSymbol *methodSymbol): methodSymbol(methodSymbol) {
    if (!methodSymbol->genericArgs.empty()) {
        throw std::runtime_error("Generic function cannot be invoked directly");
    }
    inferredType = methodSymbol->returnType;
}
