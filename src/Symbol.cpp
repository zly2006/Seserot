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

#include "Parser.h"

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

Seserot::TraitSymbol::TraitSymbol(Seserot::Scope *scope, const std::string &name, Seserot::Modifiers modifiers,
        std::vector<ClassSymbol> genericArgs, std::vector<TraitSymbol *> fathers):
        SymbolWithChildren(scope, Trait, name, nullptr),
        modifiers(modifiers),
        genericArgs(std::move(genericArgs)),
        fathers(std::move(fathers)) {}

bool Seserot::TraitSymbol::after(Seserot::TraitSymbol *symbol) {
    for (const auto &item: fathers) {
        if (item == symbol || item->after(symbol)) return true;
    }
    return false;
}

bool Seserot::TraitSymbol::afterOrEqual(Seserot::TraitSymbol *symbol) {
    return this == symbol || after(symbol);
}

Seserot::ClassSymbol::ClassSymbol(Seserot::Scope *scope, const std::string &name, std::vector<ClassSymbol> genericArgs,
        Seserot::ClassSymbol *closureFather, Modifiers modifiers, std::vector<TraitSymbol *> fathers):
        TraitSymbol(scope, name, modifiers, std::move(genericArgs), std::move(fathers)), closureFather(closureFather) {
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
        Seserot::Scope *scope, const std::string &name, Seserot::Symbol *father, Seserot::Modifiers modifiers):
        Symbol(scope, Property, name, father), modifiers(modifiers) {}

Seserot::VariableSymbol::VariableSymbol(
        Scope *scope, const std::string &name, Symbol *father, Modifiers modifiers, Seserot::TraitSymbol *returnType):
        Symbol(scope, Variable, name, father), modifiers(modifiers), returnType(returnType) {}

Seserot::MethodSymbol::MethodSymbol(Scope *scope, const std::string &name, Modifiers modifiers,
        std::vector<ClassSymbol> genericArgs, std::vector<VariableSymbol> args, TraitSymbol *returnType):
        SymbolWithChildren(scope, Method, name, nullptr),
        modifiers(modifiers),
        llvmFunction(nullptr),
        genericArgs(std::move(genericArgs)),
        args(std::move(args)),
        stackSize(0),
        returnType(returnType) {}

Seserot::MethodSymbol::MethodSymbol(Scope *scope, const std::string &name, Modifiers modifiers,
        std::vector<ClassSymbol> genericArgs, std::vector<VariableSymbol> args, TraitSymbol *returnType,
        const Parser &parser):
        SymbolWithChildren(scope, Method, name, nullptr),
        modifiers(modifiers),
        args(std::move(args)),
        stackSize(0),
        returnType(returnType) {
    if (genericArgs.empty()) {
        // todo: generate llvm function
        std::vector<llvm::Type *> args;
        args.reserve(args.size());
        for (const auto &item: this->args) {
            if (item.modifiers & Vararg) {
            } else
                args.push_back(parser.getLLVMType(item.returnType));
        }

        llvm::FunctionType *pFunctionType =
                llvm::FunctionType::get(llvm::Type::getDoubleTy(*parser.thisContext), args, false);

        llvmFunction = llvm::Function::Create(pFunctionType, llvm::Function::ExternalLinkage, name, *parser.thisModule);

        unsigned Idx = 0;
        for (auto &Arg: llvmFunction->args()) Arg.setName(this->args[Idx++].name);

        this->genericArgs = std::move(genericArgs);
    }
}

Seserot::MethodSymbol *Seserot::MethodSymbol::specialize(std::vector<ClassSymbol *> symbols, const Parser &parser) {
    if (symbols.size() != genericArgs.size() || genericArgs.empty()) {
        throw std::invalid_argument("This function is not generic or generic args do not match.");
    }
    size_t o = 0;
    for (const auto &item: symbols) o |= (size_t)item;
    if (!o) {
        // the same as this
        return this;
    }

    std::vector<ClassSymbol> newGenericArgs;
    std::vector<VariableSymbol> newArgs = args;
    TraitSymbol *newReturnType = returnType;
    for (int i = 0; i < symbols.size(); ++i) {
        ClassSymbol *before;
        ClassSymbol *after;
        if (symbols[i]) {
            before = &genericArgs[i];
            after = symbols[i];
        } else {
            newGenericArgs.push_back(genericArgs[i]);
            before = &genericArgs[i];
            after = &newGenericArgs.back();
        }
        if (returnType == before) {
            newReturnType = after;
        }
        for (auto &item: newArgs) {
            if (item.returnType == before) item.returnType = after;
        }
    }
    return new MethodSymbol(scope, name, modifiers, newGenericArgs, newArgs, newReturnType, parser);
}

std::optional<std::vector<size_t>> Seserot::MethodSymbol::match(
        std::vector<VariableSymbol> params, std::vector<TraitSymbol *> classes) {
    if (params.empty() && classes.empty()) {
        // 两个都是空的，那就是完全匹配
        return std::vector<size_t>();
    }
    if (params.empty()) {
        // 参数空，但是输入不空
        return {};
    }
    auto matchSingle = [this](TraitSymbol *param, TraitSymbol *cls) {
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
        if (it->modifiers & Vararg) {
            for (size_t i = 0; i <= classes.size() - index; ++i) {
                size_t varargPackArgCount = classes.size() - index - i;
                bool goon = true;
                auto retCopy = ret;
                for (int j = 0; j < varargPackArgCount; ++j) {
                    if (!matchSingle(it->returnType, classes[index + j])) {
                        // 如果varargPackArgCount之中有不满足的参数就break，匹配失败
                        goon = false;
                        break;
                    }
                    ret.push_back(it - params.begin());
                }
                if (!goon) {
                    // 匹配失败
                    ret = retCopy;
                    continue;
                }
                std::vector<TraitSymbol *> subClasses;
                std::vector<VariableSymbol> subParams;
                subClasses.resize(classes.size() - varargPackArgCount - index);  // varargPackArgCount = 0 to all, all =
                subParams.reserve(params.end() - it - 1);
                std::copy_n(classes.begin() + index + varargPackArgCount, subClasses.size(), subClasses.begin());
                for (auto iter = it + 1; iter != params.end(); iter++) {
                    subParams.push_back(*iter);
                }
                auto r = match(subParams, subClasses);
                if (r == std::nullopt) {
                    // 匹配失败
                    ret = retCopy;
                    continue;
                }
                std::for_each(r->begin(), r->end(), [&](size_t &item) {
                    // item 本来的值
                    // 1 当前的这个参数
                    // it - params.begin() 这个参数之前的参数数量
                    ret.push_back(item + 1 + (it - params.begin()));
                });
                // 匹配成功，因为剩下的参数都匹配完了，这里直接return
                return ret;
            }
        } else {
            if (matchSingle(it->returnType, classes[index])) {
                // 使得 ret[index] = indexOfParams，需要保证每次都这么push
                ret.push_back(it - params.begin());
                index++;
            } else {
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

Seserot::NamespaceSymbol::NamespaceSymbol(Seserot::Scope *scope, const std::string &name):
        SymbolWithChildren(scope, Namespace, name, nullptr) {}

bool Seserot::NamespaceSymbol::operator==(const Seserot::NamespaceSymbol &rhs) const {
    return name == rhs.name;
}

Seserot::SymbolWithChildren::SymbolWithChildren(
        Seserot::Scope *scope, Seserot::Symbol::Type type, const std::string &name, Seserot::Scope *childScope):
        Symbol(scope, type, name, nullptr), childScope(childScope) {}

Seserot::Symbol::Symbol(Seserot::Scope *scope, Seserot::Symbol::Type type, std::string name, Seserot::Symbol *father):
        scope(scope), type(type), name(std::move(name)), father(father) {}
