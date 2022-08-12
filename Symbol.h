//
// Created by 赵李言 on 2022/8/12.
//

#ifndef SESEROT_GEN0_SYMBOL_H
#define SESEROT_GEN0_SYMBOL_H

#include <utility>

#include "BasicStructures.h"
namespace Seserot {
    struct Symbol {
        enum Type {
            Class,
            GenericClass,
            Method,
            Property,
            Value,
            Variable,
            Namespace,
        };
        std::string name;
        Scope *scope;
        Symbol* father;
        Type type;

        explicit Symbol(Seserot::Scope *scope, Type type, std::string name, Symbol *father)
                : scope(scope), type(type), name(std::move(name)), father(father) {}
    };
    struct SymbolWithChildren: Symbol{
        SymbolWithChildren(Scope *scope,
                           Type type,
                           const std::string &name,
                           Scope *childScope)
                           : Symbol(scope, type, name, nullptr), childScope(childScope) {}
        Scope* childScope;
    };
    struct NamespaceSymbol: SymbolWithChildren {
        NamespaceSymbol(Scope *scope,
                        const std::string &name)
                : SymbolWithChildren(scope, Namespace, name, nullptr) {}
    };
    struct ClassSymbol: SymbolWithChildren {
        ClassSymbol(Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs, bool isStatic,
                    ClassSymbol *closureFather, bool isFinal)
                : SymbolWithChildren(scope, Class, name, nullptr),
                  genericArgs(std::move(genericArgs)), isStatic(isStatic), closureFather(closureFather), isFinal(isFinal) {}

        std::vector<ClassSymbol> genericArgs;
        bool isStatic;
        ClassSymbol* closureFather;
        bool isFinal;
        [[nodiscard]] std::string toString() const {
            std::string ret;
            if (isStatic) ret += "static ";
            if (isFinal) ret += "final ";
            if (closureFather != nullptr) ret += closureFather->name + "::";
            ret += name;
            return ret;
        }
    };
}

#endif //SESEROT_GEN0_SYMBOL_H
