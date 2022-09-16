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
        Partial = 128,
        Mutable = 256,
        PrivateProtected = 512,
        PrivateInternal = 1024,
        Inner = 2048,
    };

    bool isPrivateModifier(Modifiers);

    bool isPublicModifier(Modifiers);

    bool isInternalModifier(Modifiers);

    bool isProtectedModifier(Modifiers);

    struct Symbol {
        enum Type {
            Class = 1,
            Trait = 2,
            Method = 4,
            Property = 8,
            Value = 16,
            Variable = 32,
            Namespace = 64,
        };
        std::string name;
        Scope *scope;
        Symbol *father;
        Type type;

        Symbol(Seserot::Scope *scope, Type type, std::string name, Symbol *father);
    };

    struct SymbolWithChildren : Symbol {
        SymbolWithChildren(
                Scope *scope,
                Type type,
                const std::string &name,
                Scope *childScope);

        Scope *childScope;
    };

    struct NamespaceSymbol : SymbolWithChildren {
        NamespaceSymbol(
                Scope *scope,
                const std::string &name);
    };

    struct TraitSymbol;

    struct ClassSymbol : SymbolWithChildren {
        ClassSymbol(
                Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs, ClassSymbol *closureFather,
                Modifiers modifiers);

        ClassSymbol(
                Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs, ClassSymbol *closureFather,
                int modifiers);

        Modifiers modifiers;
        std::vector<ClassSymbol> genericArgs;
        /**
         * 弃用
         */
        ClassSymbol *closureFather;
        std::vector<TraitSymbol*> traits;
        size_t size = 0;

        [[nodiscard]] std::string toString() const;
    };

    struct TraitSymbol : SymbolWithChildren {
        TraitSymbol(Scope *scope, const std::string &name, Modifiers modifiers);

        Modifiers modifiers;
        std::vector<ClassSymbol> genericArgs;
    };

    struct MethodSymbol : SymbolWithChildren {
        MethodSymbol(
                Scope *scope, const std::string &name, Modifiers modifiers, std::vector<ClassSymbol> genericArgs,
                std::vector<ClassSymbol *> args, ClassSymbol *returnType);

        Modifiers modifiers;
        std::vector<ClassSymbol> genericArgs;// holder of generic types (uch as T, P)
        std::vector<ClassSymbol *> args;
        ClassSymbol *returnType;
        size_t stackSize;
    };

    struct VariableSymbol : Symbol {
        VariableSymbol(Scope *scope, const std::string &name, Symbol *father, Modifiers modifiers);
        ClassSymbol* returnType = nullptr;
        Modifiers modifiers;
    };

    struct PropertySymbol : Symbol {
        PropertySymbol(
                Scope *scope, const std::string &name, Symbol *father,
                Modifiers modifiers);
        Modifiers modifiers;
        ClassSymbol* returnType = nullptr;
        MethodSymbol* getter = nullptr;
        MethodSymbol* setter = nullptr;
    };
}

#endif //SESEROT_GEN0_SYMBOL_H
