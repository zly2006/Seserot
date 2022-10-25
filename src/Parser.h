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

#ifndef SESEROT_GEN0_PARSER_H
#define SESEROT_GEN0_PARSER_H

#include "BasicStructures.h"
#include <vector>
#include <llvm/CodeGen/ValueTypes.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalIFunc.h>
#include "ErrorTable.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include <map>
#include <unordered_map>
#include <set>
#include <any>
#include <optional>
#include "ast/ASTNode.h"
#include "ast/IntegerConstantNode.h"

namespace Seserot {
    class Parser {
    private:
        friend int::main(int, char **, char **);

        using token_iter = std::vector<Token>::iterator;
    public:
        Parser(std::vector<Token> tokens, ErrorTable &errorTable);

        void parse();

        void scan();

        size_t generateStack(MethodSymbol *);

        void parseReference();

        llvm::Function *generateFunctionDefinition(MethodSymbol *symbol, const std::string &name);

        void generateFunctionIR(llvm::Function *symbol, Scope *definition);

        llvm::Type *getLLVMType(TraitSymbol *traitSymbol) const;

        /*void processGeneric();
        void generateTypeVar();
        void parseFunctionAST();*/
        Modifiers parseModifiers(std::vector<Token>::iterator it, Modifiers = None);

        SymbolTable symbolTable = SymbolTable(&root);
        ErrorTable &errorTable;
        FileScope root;
        std::vector<Token> tokens;
        llvm::LLVMContext *thisContext = nullptr;
        llvm::IRBuilder<> *irBuilder = nullptr;
        llvm::Module *thisModule = nullptr;

        void setupCodegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module);

    private:
        void importSymbols(const std::vector<std::string_view> &symbols);

        Token &read(size_t &);

        void appendError(size_t code, const std::string &message, const SourcePosition &position,
                         const std::string &category = "error");

        static ClassSymbol *currentClassSymbol(Symbol *);

        static MethodSymbol *currentMethodSymbol(Symbol *);

        static NamespaceSymbol *currentNamespaceSymbol(Symbol *);

        AST::ASTNode *parseExpression(token_iter &tokenIter, char untilBracket = 0);

        AST::ASTNode *parseIf(token_iter &tokenIter);

        AST::ASTNode *parseWhile(token_iter &tokenIter);

        AST::ASTNode *parseFor(token_iter &tokenIter);

        AST::ASTNode *parseBlock(token_iter &tokenIter);

        static AST::IntegerConstantNode *string2FitNumber(const std::string &str, size_t &ret, bool = false);

        Token &expectIdentifier(size_t &pos);

        Token &expectOperator(size_t &pos);

        bool expectIdentifier(size_t &pos, const std::string &content);

        template<class T>
        std::optional<T> convertToNumber(const std::string &str);

        std::multimap<Token *, Symbol *> reference;
        std::map<Token *, Scope *> token2scope;
        std::vector<MethodSymbol> specializedGenericMethod;
        std::vector<ClassSymbol> specializedGenericClass;
        llvm::Type *dynamic{};
        const std::set<std::string> modifiers{
                "final",
                "static",
                "public",
                "protected",
                "internal",
                "private",
                "mutable",
                "immutable",
                "inner",
                "partial",

        };
        const std::set<std::string> modifierAccessibility{
                "public",
                "protected",
                "internal",
                "private"
        };
        const std::set<std::string> modifierMutable{
                "mutable",
                "immutable"
        };
    };

} // Seserot

#endif //SESEROT_GEN0_PARSER_H
