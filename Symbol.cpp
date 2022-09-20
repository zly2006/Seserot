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

#include "Symbol.h"

bool Seserot::isPrivateModifier(Modifiers modifiers) {
    return (modifiers & PrivateProtected) && (modifiers & PrivateInternal);
}

bool Seserot::isPublicModifier(Seserot::Modifiers modifiers) {
    return !(modifiers & PrivateProtected) && !(modifiers & PrivateInternal);
}

bool Seserot::isProtectedModifier(Seserot::Modifiers modifiers) {
    return (modifiers & PrivateProtected) && !(modifiers & PrivateInternal);
}

bool Seserot::isInternalModifier(Seserot::Modifiers modifiers) {
    return !(modifiers & PrivateProtected) && (modifiers & PrivateInternal);
}

Seserot::TraitSymbol::TraitSymbol(
        Seserot::Scope *scope, const std::string &name,
        Seserot::Modifiers modifiers) : SymbolWithChildren(scope, Trait, name, nullptr), modifiers(modifiers) {}

Seserot::ClassSymbol::ClassSymbol(
        Seserot::Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs,
        Seserot::ClassSymbol *closureFather, int modifiers)
        : SymbolWithChildren(scope, Class, name, nullptr),
          genericArgs(std::move(genericArgs)), closureFather(closureFather), modifiers((Modifiers) modifiers) {}

Seserot::ClassSymbol::ClassSymbol(
        Seserot::Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs,
        Seserot::ClassSymbol *closureFather, Seserot::Modifiers modifiers)
        : SymbolWithChildren(scope, Class, name, nullptr),
          genericArgs(std::move(genericArgs)), closureFather(closureFather), modifiers(modifiers) {}

std::string Seserot::ClassSymbol::toString() const {
    std::string ret;
    if (modifiers & Modifiers::Static) ret += "static ";
    if (modifiers & Modifiers::Final) ret += "final ";
    if (closureFather != nullptr) ret += closureFather->name + "::";
    ret += name;
    return ret;
}

bool Seserot::ClassSymbol::operator==(Seserot::ClassSymbol *another) {
    if (modifiers != another->modifiers) return false;
    if (size != another->size) return false;
    if (scope != another->scope) return false;
    if (traits != another->traits) return false;
    if (genericArgs.size() != another->genericArgs.size()) return false;
    return true;
}

Seserot::PropertySymbol::PropertySymbol(
        Seserot::Scope *scope, const std::string &name, Seserot::Symbol *father, Seserot::Modifiers modifiers) : Symbol(scope, Property, name, father), modifiers(modifiers) {}

Seserot::VariableSymbol::VariableSymbol(
        Seserot::Scope *scope, const std::string &name, Seserot::Symbol *father, Seserot::Modifiers modifiers)
        : Symbol(scope, Variable, name, father), modifiers(modifiers) {}

Seserot::MethodSymbol::MethodSymbol(
        Seserot::Scope *scope, const std::string &name, Seserot::Modifiers modifiers,
        std::vector<ClassSymbol> genericArgs, std::vector<ClassSymbol *> args, Seserot::ClassSymbol *returnType)
        : SymbolWithChildren(scope, Method, name, nullptr), modifiers(modifiers),
          genericArgs(std::move(genericArgs)),
          args(std::move(args)), stackSize(0), returnType(returnType) {}

Seserot::NamespaceSymbol::NamespaceSymbol(Seserot::Scope *scope, const std::string &name)
        : SymbolWithChildren(scope, Namespace, name, nullptr) {}

Seserot::SymbolWithChildren::SymbolWithChildren(
        Seserot::Scope *scope, Seserot::Symbol::Type type, const std::string &name, Seserot::Scope *childScope)
        : Symbol(scope, type, name, nullptr), childScope(childScope) {}

Seserot::Symbol::Symbol(Seserot::Scope *scope, Seserot::Symbol::Type type, std::string name, Seserot::Symbol *father)
        : scope(scope), type(type), name(std::move(name)), father(father) {}
