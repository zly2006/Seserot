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

#include <utility>

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
        Seserot::Scope *scope, const std::string &name, Seserot::Modifiers modifiers,
        std::vector<ClassSymbol> genericArgs, std::vector<TraitSymbol *> fathers)
        : SymbolWithChildren(scope, Trait, name, nullptr), modifiers(modifiers), genericArgs(std::move(genericArgs)),
          fathers(std::move(fathers)) {}

bool Seserot::TraitSymbol::after(Seserot::TraitSymbol *symbol) {
    for (const auto &item: fathers) {
        if (item == symbol ||
            item->after(symbol))
            return true;
    }
    return false;
}

bool Seserot::TraitSymbol::afterOrEqual(Seserot::TraitSymbol *symbol) {
    return this == symbol || after(symbol);
}

Seserot::ClassSymbol::ClassSymbol(
        Seserot::Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs,
        Seserot::ClassSymbol *closureFather, Modifiers modifiers)
        : TraitSymbol(scope, name, modifiers, std::move(genericArgs), {}),
          closureFather(closureFather) {
    type = Class;
}

std::string Seserot::ClassSymbol::toString() const {
    std::string ret;
    if (modifiers & Modifiers::Static) ret += "static ";
    if (modifiers & Modifiers::Final) ret += "final ";
    // Kokomi~~~
    if (closureFather != nullptr) ret += closureFather->name + "::";
    ret += name;
    return ret;
}

bool Seserot::ClassSymbol::operator==(Seserot::ClassSymbol *another) {
    if (modifiers != another->modifiers) return false;
    if (size != another->size) return false;
    if (scope != another->scope) return false;
    if (genericArgs.size() != another->genericArgs.size()) return false;
    if (fathers != another->fathers) return false;
    return true;
}

Seserot::PropertySymbol::PropertySymbol(
        Seserot::Scope *scope, const std::string &name, Seserot::Symbol *father, Seserot::Modifiers modifiers) : Symbol(
        scope, Property, name, father), modifiers(modifiers) {}

Seserot::VariableSymbol::VariableSymbol(
        Scope *scope, const std::string &name, Symbol *father, Modifiers modifiers)
        : Symbol(scope, Variable, name, father), modifiers(modifiers) {}

Seserot::MethodSymbol::MethodSymbol(
        Scope *scope, const std::string &name, Modifiers modifiers,
        std::vector<ClassSymbol> genericArgs, std::vector<TraitSymbol *> args, TraitSymbol *returnType)
        : SymbolWithChildren(scope, Method, name, nullptr), modifiers(modifiers),
          genericArgs(std::move(genericArgs)),
          args(std::move(args)), stackSize(0), returnType(returnType) {}

Seserot::MethodSymbol *Seserot::MethodSymbol::specialize(std::vector<ClassSymbol *> symbols) {
    if (symbols.size() != genericArgs.size() || genericArgs.empty()) {
        throw std::invalid_argument("This function is not generic or generic args do not match.");
    }
    size_t o = 0;
    for (const auto &item: symbols)
        o |= (size_t) item;
    if (!o) {
        // the same as this
        return this;
    }
    auto *pNew = new MethodSymbol(scope, name, modifiers, {}, args, nullptr);
    for (int i = 0; i < symbols.size(); ++i) {
        ClassSymbol *before;
        ClassSymbol *after;
        if (symbols[i]) {
            before = &genericArgs[i];
            after = symbols[i];
        }
        else {
            pNew->genericArgs.push_back(genericArgs[i]);
            before = &genericArgs[i];
            after = &(pNew->genericArgs).back();
        }
        if (returnType == before) {
            pNew->returnType = after;
        }
        for (auto &item: pNew->args) {
            if (item == before)
                item = after;
        }
    }
    return pNew;
}

std::optional<std::vector<size_t>> Seserot::MethodSymbol::match
        (std::vector<TraitSymbol *> params, std::vector<TraitSymbol *> classes) {
    if (params.empty() && classes.empty()) {
        return std::vector<size_t>();
    }
    auto matchSingle = [this](TraitSymbol* param, TraitSymbol* cls) {
        if (cls->afterOrEqual(param)) return true;
        for (const auto &item: genericArgs) {
            if (&item == param) return true;
        }
        // I LOVE U TOO LOVE U TOO
        return false;
    };
    std::vector<size_t> ret;
    size_t index = 0;
    for (auto it = params.begin(); it != params.end(); it++) {
        if ((*it)->modifiers & Vararg) {
            for (size_t i = 0; i <= classes.size() - index; ++i) {
                bool goon = true;
                for (int j = 0; j < i; ++j) {
                    // i是vararg包含的数量
                    if (!matchSingle(*it, classes[index + j])) {
                        // 如果i之后有不满足的参数就break，匹配失败
                        goon = false;
                        break;
                    }
                    ret.push_back(it - params.begin());
                }
                if (!goon) {
                    return {};
                }
                std::vector<TraitSymbol *> subClasses;
                std::vector<TraitSymbol *> subParams;
                subParams.resize(params.end() - it - 1);
                subClasses.resize(classes.size() - i - index); // i = 0 to all, all =
                std::copy_n(classes.begin() + index + i, subClasses.size(), subClasses.begin());
                std::copy(it + 1, params.end(), subParams.begin());
                auto r = match(subParams, subClasses);
                if (r == std::nullopt) continue;
                std::for_each(ret.begin(), ret.end(), [&](size_t &item) {
                    ret.push_back(item + (it - params.begin()));
                });
                return ret;
            }
        }
        else {
            if (matchSingle(*it, classes[index])) {
                // 使得 ret[index] = indexOfParams，需要保证每次都这么push
                ret.push_back(it - params.begin());
                index++;
            }
            else {
                return {};
            }
        }
    }
    if (ret.size() != classes.size()) {
        // failed
        return {};
    }
    return ret;
}

std::optional<std::vector<size_t>> Seserot::MethodSymbol::match(std::vector<TraitSymbol *> classes) {
    return match(args, std::move(classes));
}

Seserot::NamespaceSymbol::NamespaceSymbol(Seserot::Scope *scope, const std::string &name)
        : SymbolWithChildren(scope, Namespace, name, nullptr) {}

Seserot::SymbolWithChildren::SymbolWithChildren(
        Seserot::Scope *scope, Seserot::Symbol::Type type, const std::string &name, Seserot::Scope *childScope)
        : Symbol(scope, type, name, nullptr), childScope(childScope) {}

Seserot::Symbol::Symbol(Seserot::Scope *scope, Seserot::Symbol::Type type, std::string name, Seserot::Symbol *father)
        : scope(scope), type(type), name(std::move(name)), father(father) {}
