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
        Infix = 4096,
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

        bool operator==(ClassSymbol* another);

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
