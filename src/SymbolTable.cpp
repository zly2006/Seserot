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

    TraitSymbol *SymbolTable::Number = nullptr;
    ClassSymbol *SymbolTable::String = nullptr;
    ClassSymbol *SymbolTable::Void = nullptr;
    ClassSymbol *SymbolTable::Int = nullptr;
    ClassSymbol *SymbolTable::Long = nullptr;
    ClassSymbol *SymbolTable::Short = nullptr;
    ClassSymbol *SymbolTable::Float = nullptr;
    ClassSymbol *SymbolTable::Double = nullptr;
    ClassSymbol *SymbolTable::Char = nullptr;
    ClassSymbol *SymbolTable::Boolean = nullptr;
    ClassSymbol *SymbolTable::Byte = nullptr;
    ClassSymbol *SymbolTable::Object = nullptr;
    ClassSymbol *SymbolTable::Array = nullptr;
    ClassSymbol *SymbolTable::Trait = nullptr;
    ClassSymbol *SymbolTable::Class = nullptr;
    ClassSymbol *SymbolTable::Enum = nullptr;
    ClassSymbol *SymbolTable::Function = nullptr;

    SymbolTable builtinTable;

    void initBuiltinTable() {
        builtinTable.importedTables.clear();
#pragma region Basuc types
        builtinTable.symbols.emplace("C::.Number;", std::make_unique<TraitSymbol>(
                nullptr,
                "Number",
                (Modifiers) (Static | ValueType),
                std::vector<ClassSymbol>(),
                std::vector<TraitSymbol *>()
        ));
        SymbolTable::Number = dynamic_cast<TraitSymbol *>(builtinTable.symbols["C::.Number"].get());
        builtinTable.symbols.emplace("C::.String;", std::make_unique<ClassSymbol>(
                nullptr,
                "String",
                std::vector<ClassSymbol>(),
                nullptr,
                Static,
                std::vector<TraitSymbol *>()
        ));
        SymbolTable::String = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.String"].get());
        builtinTable.symbols.emplace("C::.Void;", std::make_unique<ClassSymbol>(
                nullptr,
                "Void",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | Final),
                std::vector<TraitSymbol *>()
        ));
        SymbolTable::Void = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Void"].get());
        builtinTable.symbols.emplace("C::.Int;", std::make_unique<ClassSymbol>(
                nullptr,
                "Int",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{SymbolTable::Number}
        ));
        SymbolTable::Int = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Int"].get());
        builtinTable.symbols.emplace("C::.Long;", std::make_unique<ClassSymbol>(
                nullptr,
                "Long",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{SymbolTable::Number}
        ));
        SymbolTable::Long = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Long"].get());
        builtinTable.symbols.emplace("C::.Short;", std::make_unique<ClassSymbol>(
                nullptr,
                "Short",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{SymbolTable::Number}
        ));
        SymbolTable::Short = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Short"].get());
        builtinTable.symbols.emplace("C::.Float;", std::make_unique<ClassSymbol>(
                nullptr,
                "Float",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{SymbolTable::Number}
        ));
        SymbolTable::Float = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Float"].get());
        builtinTable.symbols.emplace("C::.Double;", std::make_unique<ClassSymbol>(
                nullptr,
                "Double",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{SymbolTable::Number}
        ));
        SymbolTable::Double = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Double"].get());
        builtinTable.symbols.emplace("C::.Boolean;", std::make_unique<ClassSymbol>(
                nullptr,
                "Boolean",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>()
        ));
        SymbolTable::Boolean = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Boolean"].get());
        builtinTable.symbols.emplace("C::.Char;", std::make_unique<ClassSymbol>(
                nullptr,
                "Char",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>()
        ));
        SymbolTable::Char = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Char"].get());
        builtinTable.symbols.emplace("C::.Byte;", std::make_unique<ClassSymbol>(
                nullptr,
                "Byte",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>()
        ));
        SymbolTable::Byte = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Byte"].get());
        builtinTable.symbols.emplace("C::.Array;", std::make_unique<ClassSymbol>(
                nullptr,
                "Array",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (None),
                std::vector<TraitSymbol *>()
        ));
        SymbolTable::Array = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Array"].get());
#pragma endregion
#pragma region Reflection
        builtinTable.symbols.emplace("C::.Trait", std::make_unique<ClassSymbol>(
                nullptr,
                "Trait",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | Final),
                std::vector<TraitSymbol *>()
        ));
        SymbolTable::Trait = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Trait"].get());
        builtinTable.symbols.emplace("C::.Trait", std::make_unique<ClassSymbol>(
                nullptr,
                "Class",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | Final),
                std::vector<TraitSymbol *>{SymbolTable::Trait}
        ));
        SymbolTable::Class = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Class"].get());
        builtinTable.symbols.emplace("C::.Function", std::make_unique<ClassSymbol>(
                nullptr,
                "Function",
                std::vector<ClassSymbol>{
                        ClassSymbol(nullptr, "P", {}, nullptr, None, {}),
                        ClassSymbol(nullptr, "T", {}, nullptr, None, {}),
                        ClassSymbol(nullptr, "+", {}, nullptr, None, {}),
                },
                nullptr,
                (Modifiers) (Static | Final),
                std::vector<TraitSymbol *>{}
        ));
        SymbolTable::Function = dynamic_cast<ClassSymbol *>(builtinTable.symbols["C::.Function"].get());
#pragma endregion
    }

    SymbolTable::SymbolTable(Scope *root) : root(root) {
        if (root) {
            if (!Number) {
                initBuiltinTable();
            }
            importedTables.push_back(&builtinTable);
        }
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
        return result;
    }

    bool SymbolTable::emplace(std::unique_ptr<Symbol> symbol) {
        if (symbols.count(symbol->signature)) {
            return false;
        }
        symbols.emplace(symbol->signature, std::move(symbol));
        return true;
    }
} // Seserot