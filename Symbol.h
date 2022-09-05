//
// Created by 赵李言 on 2022/8/12.
//

#ifndef SESEROT_GEN0_SYMBOL_H
#define SESEROT_GEN0_SYMBOL_H

#include <utility>

#include "BasicStructures.h"
namespace Seserot {
    enum Modifiers {
        None = 0,
        Async = 1,
        Final = 2,
        Static = 4,
        Abstract = 8,
        Operator = 16,
        ValueType = 32,
        Readonly = 64,
    };
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
        ClassSymbol(
                Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs, ClassSymbol *closureFather,
                Modifiers modifiers)
                : SymbolWithChildren(scope, Class, name, nullptr),
                  genericArgs(std::move(genericArgs)), closureFather(closureFather), modifiers(modifiers) {}
        ClassSymbol(
                Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs, ClassSymbol *closureFather,
                int modifiers)
                : SymbolWithChildren(scope, Class, name, nullptr),
                  genericArgs(std::move(genericArgs)), closureFather(closureFather), modifiers((Modifiers)modifiers) {}
        Modifiers modifiers;
        std::vector<ClassSymbol> genericArgs;
        /**
         * 弃用
         */
        ClassSymbol* closureFather;
        [[nodiscard]] std::string toString() const {
            std::string ret;
            if (modifiers & Modifiers::Static) ret += "static ";
            if (modifiers & Modifiers::Final) ret += "final ";
            if (closureFather != nullptr) ret += closureFather->name + "::";
            ret += name;
            return ret;
        }
    };
    struct MethodSymbol: SymbolWithChildren {
        Modifiers modifiers;
        std::vector<ClassSymbol> genericArgs;// holder of generic types (uch as T, P)
        std::vector<ClassSymbol*> args;
        size_t stackSize;
    };
}

#endif //SESEROT_GEN0_SYMBOL_H
