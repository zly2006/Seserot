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
            GenericClass = 2,
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

        explicit Symbol(Seserot::Scope *scope, Type type, std::string name, Symbol *father)
                : scope(scope), type(type), name(std::move(name)), father(father) {}
    };

    struct SymbolWithChildren : Symbol {
        SymbolWithChildren(
                Scope *scope,
                Type type,
                const std::string &name,
                Scope *childScope)
                : Symbol(scope, type, name, nullptr), childScope(childScope) {}

        Scope *childScope;
    };

    struct NamespaceSymbol : SymbolWithChildren {
        NamespaceSymbol(
                Scope *scope,
                const std::string &name)
                : SymbolWithChildren(scope, Namespace, name, nullptr) {}
    };

    struct TraitSymbol;

    struct ClassSymbol : SymbolWithChildren {
        ClassSymbol(
                Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs, ClassSymbol *closureFather,
                Modifiers modifiers)
                : SymbolWithChildren(scope, Class, name, nullptr),
                  genericArgs(std::move(genericArgs)), closureFather(closureFather), modifiers(modifiers) {}

        ClassSymbol(
                Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs, ClassSymbol *closureFather,
                int modifiers)
                : SymbolWithChildren(scope, Class, name, nullptr),
                  genericArgs(std::move(genericArgs)), closureFather(closureFather), modifiers((Modifiers) modifiers) {}

        Modifiers modifiers;
        std::vector<ClassSymbol> genericArgs;
        /**
         * 弃用
         */
        ClassSymbol *closureFather;
        std::vector<TraitSymbol*> traits;
        size_t size;

        [[nodiscard]] std::string toString() const {
            std::string ret;
            if (modifiers & Modifiers::Static) ret += "static ";
            if (modifiers & Modifiers::Final) ret += "final ";
            if (closureFather != nullptr) ret += closureFather->name + "::";
            ret += name;
            return ret;
        }
    };

    struct TraitSymbol : SymbolWithChildren {
        Modifiers modifiers;
        std::vector<ClassSymbol> genericArgs;
    };

    struct MethodSymbol : SymbolWithChildren {
        MethodSymbol(
                Scope *scope, const std::string &name, Modifiers modifiers, std::vector<ClassSymbol> genericArgs,
                std::vector<ClassSymbol *> args, ClassSymbol *returnType)
                : SymbolWithChildren(scope, Method, name, nullptr), modifiers(modifiers),
                  genericArgs(std::move(genericArgs)),
                  args(std::move(args)), stackSize(0), returnType(returnType) {}

        Modifiers modifiers;
        std::vector<ClassSymbol> genericArgs;// holder of generic types (uch as T, P)
        std::vector<ClassSymbol *> args;
        ClassSymbol *returnType;
        size_t stackSize;
    };

    struct VariableSymbol : Symbol {
        VariableSymbol(Scope *scope, const std::string &name, Symbol *father, Modifiers modifiers)
                : Symbol(scope, Variable, name, father), modifiers(modifiers) {}
        ClassSymbol* returnType = nullptr;
        Modifiers modifiers;
    };

    struct PropertySymbol : Symbol {
        PropertySymbol(
                Scope *scope, const std::string &name, Symbol *father,
                Modifiers modifiers) : Symbol(scope, Property, name, father), modifiers(modifiers) {}
        Modifiers modifiers;
        ClassSymbol* returnType = nullptr;
        MethodSymbol* getter = nullptr;
        MethodSymbol* setter = nullptr;
    };
}

#endif //SESEROT_GEN0_SYMBOL_H
