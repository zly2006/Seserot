//
// Created by 赵李言 on 2022/9/6.
//
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
