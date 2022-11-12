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

#include "SymbolTable.h"

namespace Seserot {
    static std::map<std::string, SymbolTable *> symbolTables;

    TraitSymbol *BuiltinSymbols::Number;
    ClassSymbol *BuiltinSymbols::String;
    ClassSymbol *BuiltinSymbols::Void;
    ClassSymbol *BuiltinSymbols::Int;
    ClassSymbol *BuiltinSymbols::Long;
    ClassSymbol *BuiltinSymbols::Short;
    ClassSymbol *BuiltinSymbols::Float;
    ClassSymbol *BuiltinSymbols::Double;
    ClassSymbol *BuiltinSymbols::Char;
    ClassSymbol *BuiltinSymbols::Boolean;
    ClassSymbol *BuiltinSymbols::Byte;
    ClassSymbol *BuiltinSymbols::Object;
    ClassSymbol *BuiltinSymbols::Array;
    ClassSymbol *BuiltinSymbols::Trait;
    ClassSymbol *BuiltinSymbols::Class;
    ClassSymbol *BuiltinSymbols::Enum;
    ClassSymbol *BuiltinSymbols::Function;
    SymbolTable BuiltinSymbols::builtinTable;

    void BuiltinSymbols::init() {
        if (Number != nullptr) {
            return;
        }
        builtinTable.importedTables.clear();
        builtinTable.symbols.clear();
#pragma region Basuc types
        builtinTable.symbols.emplace(
                "C::.Number;", std::make_unique<TraitSymbol>(nullptr, "Number", (Modifiers)(Static | ValueType),
                                       std::vector<ClassSymbol>(), std::vector<TraitSymbol *>()));
        BuiltinSymbols::Number = dynamic_cast<TraitSymbol *>(builtinTable.symbols["C::.Number;"].get());
        builtinTable.symbols.emplace(
                "C::.String;", std::make_unique<ClassSymbol>(nullptr, "String", std::vector<ClassSymbol>(), nullptr,
                                       Static, std::vector<TraitSymbol *>()));
        BuiltinSymbols::String = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.String;"].get());
        builtinTable.symbols.emplace(
                "C::.Void;", std::make_unique<ClassSymbol>(nullptr, "Void", std::vector<ClassSymbol>(), nullptr,
                                     (Modifiers)(Static | Final), std::vector<TraitSymbol *>()));
        BuiltinSymbols::Void = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Void;"].get());
        builtinTable.symbols.emplace("C::.Int;",
                std::make_unique<ClassSymbol>(nullptr, "Int", std::vector<ClassSymbol>(), nullptr,
                        (Modifiers)(Static | ValueType), std::vector<TraitSymbol *>{BuiltinSymbols::Number}));
        BuiltinSymbols::Int = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Int;"].get());
        builtinTable.symbols.emplace("C::.Long;",
                std::make_unique<ClassSymbol>(nullptr, "Long", std::vector<ClassSymbol>(), nullptr,
                        (Modifiers)(Static | ValueType), std::vector<TraitSymbol *>{BuiltinSymbols::Number}));
        BuiltinSymbols::Long = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Long;"].get());
        builtinTable.symbols.emplace("C::.Short;",
                std::make_unique<ClassSymbol>(nullptr, "Short", std::vector<ClassSymbol>(), nullptr,
                        (Modifiers)(Static | ValueType), std::vector<TraitSymbol *>{BuiltinSymbols::Number}));
        BuiltinSymbols::Short = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Short;"].get());
        builtinTable.symbols.emplace("C::.Float;",
                std::make_unique<ClassSymbol>(nullptr, "Float", std::vector<ClassSymbol>(), nullptr,
                        (Modifiers)(Static | ValueType), std::vector<TraitSymbol *>{BuiltinSymbols::Number}));
        BuiltinSymbols::Float = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Float;"].get());
        builtinTable.symbols.emplace("C::.Double;",
                std::make_unique<ClassSymbol>(nullptr, "Double", std::vector<ClassSymbol>(), nullptr,
                        (Modifiers)(Static | ValueType), std::vector<TraitSymbol *>{BuiltinSymbols::Number}));
        BuiltinSymbols::Double = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Double;"].get());
        builtinTable.symbols.emplace(
                "C::.Boolean;", std::make_unique<ClassSymbol>(nullptr, "Boolean", std::vector<ClassSymbol>(), nullptr,
                                        (Modifiers)(Static | ValueType), std::vector<TraitSymbol *>()));
        BuiltinSymbols::Boolean = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Boolean;"].get());
        builtinTable.symbols.emplace(
                "C::.Char;", std::make_unique<ClassSymbol>(nullptr, "Char", std::vector<ClassSymbol>(), nullptr,
                                     (Modifiers)(Static | ValueType), std::vector<TraitSymbol *>()));
        BuiltinSymbols::Char = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Char;"].get());
        builtinTable.symbols.emplace(
                "C::.Byte;", std::make_unique<ClassSymbol>(nullptr, "Byte", std::vector<ClassSymbol>(), nullptr,
                                     (Modifiers)(Static | ValueType), std::vector<TraitSymbol *>()));
        BuiltinSymbols::Byte = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Byte;"].get());
        builtinTable.symbols.emplace(
                "C::.Array;", std::make_unique<ClassSymbol>(nullptr, "Array", std::vector<ClassSymbol>(), nullptr,
                                      (Modifiers)(None), std::vector<TraitSymbol *>()));
        BuiltinSymbols::Array = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Array;"].get());
#pragma endregion
#pragma region Reflection
        builtinTable.symbols.emplace(
                "C::.Trait;", std::make_unique<ClassSymbol>(nullptr, "Trait", std::vector<ClassSymbol>(), nullptr,
                                      (Modifiers)(Static | Final), std::vector<TraitSymbol *>()));
        BuiltinSymbols::Trait = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Trait;"].get());
        builtinTable.symbols.emplace(
                "C::.Class;", std::make_unique<ClassSymbol>(nullptr, "Class", std::vector<ClassSymbol>(), nullptr,
                                      (Modifiers)(Static | Final), std::vector<TraitSymbol *>{BuiltinSymbols::Trait}));
        BuiltinSymbols::Class = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Class;"].get());
        builtinTable.symbols.emplace(
                "C::.Function;", std::make_unique<ClassSymbol>(nullptr, "Function",
                                         std::vector<ClassSymbol>{
                                                 ClassSymbol(nullptr, "P", {}, nullptr, None, {}),
                                                 ClassSymbol(nullptr, "T", {}, nullptr, None, {}),
                                                 ClassSymbol(nullptr, "+", {}, nullptr, None, {}),
                                         },
                                         nullptr, (Modifiers)(Static | Final), std::vector<TraitSymbol *>{}));
        BuiltinSymbols::Function = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Function;"].get());
#pragma endregion
    }

    std::vector<Symbol *> SymbolTable::lookup(std::string_view name) {
        std::vector<Symbol *> result;
        for (const auto &item: symbols) {
            if (item.second->scope == nullptr || item.second->scope == root) {
                if (item.second->name == name) {
                    result.push_back(item.second.get());
                }
            }
        }
        for (const auto &item: importedTables) {
            auto temp = item->lookup(name);
            result.insert(result.end(), temp.begin(), temp.end());
        }
        return result;
    }

    std::vector<Symbol *> SymbolTable::lookup(std::string_view name, Scope *scope) {
        std::vector<Symbol *> result = lookup(name);
        for (const auto &item: symbols) {
            if (scope->inside(item.second->scope)) {
                if (item.second->name == name) {
                    result.push_back(item.second.get());
                }
            }
        }
        // check duplicate symbol
        for (int i = 0; i < result.size(); i++) {
            for (int j = i + 1; j < result.size(); j++) {
                if (result[i]->scope->inside(result[j]->scope)) {
                    result.erase(result.begin() + j);
                    j--;
                } else if (result[j]->scope->inside(result[i]->scope)) {
                    result.erase(result.begin() + i);
                    i--;
                    break;
                }
            }
        }
        return result;
    }

    bool SymbolTable::emplace(std::unique_ptr<Symbol> symbol) {
        if (symbols.count(symbol->signature)) {
            return false;
        }
        symbols.emplace(symbol->signature, std::move(symbol));
        return true;
    }

    SymbolTable::SymbolTable(Scope *root, NamespaceSymbol *symbol): root(root), currentNamespace(symbol) {
        if (!BuiltinSymbols::Number) {
            BuiltinSymbols::init();
        }
        if (root) {
            importedTables.push_back(&BuiltinSymbols::builtinTable);
        }
        if (symbol) {
            symbols.emplace(symbol->signature, symbol);
            symbolTables.emplace(symbol->signature, this);
        }
    }

    SymbolTable &getSymbolTable(const std::string &name) {
        return *symbolTables[name];
    }

    TraitSymbol *getCommonBaseClass(TraitSymbol *a, TraitSymbol *b) {
        if (a == b) {
            return a;
        }
        if (a->afterOrEqual(b)) {
            return b;
        }
        if (b->afterOrEqual(a)) {
            return a;
        }
        for (auto item: a->fathers) {
            auto *temp = getCommonBaseClass(b, item);
            if (temp) {
                return temp;
            }
        }
        return nullptr;
    }

}  // namespace Seserot
