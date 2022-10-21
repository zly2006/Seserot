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
    SymbolTable::SymbolTable(Scope *root, bool buildDefault) : root(root) {
        symbols.emplace("::.Number", std::make_unique<TraitSymbol>(
                nullptr,
                "Number",
                (Modifiers) (Static | ValueType),
                std::vector<ClassSymbol>(),
                std::vector<TraitSymbol *>()
        ));
        quickLookup.emplace("::.Number", symbols["::.Number"].get());
        symbols.emplace("::.String", std::make_unique<ClassSymbol>(
                nullptr,
                "String",
                std::vector<ClassSymbol>(),
                nullptr,
                Static,
                std::vector<TraitSymbol *>()
        ));
        quickLookup.emplace("::.String", symbols["::.String"].get());
        symbols.emplace("::.Void", std::make_unique<ClassSymbol>(
                nullptr,
                "Void",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | Final),
                std::vector<TraitSymbol *>()
        ));
        quickLookup.emplace("::.Void", symbols["::.Void"].get());
        symbols.emplace("::.Int", std::make_unique<ClassSymbol>(
                nullptr,
                "Int",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{(TraitSymbol *) symbols["::.Number"].get()}
        ));
        quickLookup.emplace("::.Int", symbols["::.Int"].get());
        symbols.emplace("::.Long", std::make_unique<ClassSymbol>(
                nullptr,
                "Long",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{(TraitSymbol *) symbols["::.Number"].get()}
        ));
        quickLookup.emplace("::.Long", symbols["::.Long"].get());
        symbols.emplace("::.Float", std::make_unique<ClassSymbol>(
                nullptr,
                "Float",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{(TraitSymbol *) symbols["::.Number"].get()}
        ));
        quickLookup.emplace("::.Float", symbols["::.Float"].get());
        symbols.emplace("::.Double", std::make_unique<ClassSymbol>(
                nullptr,
                "Double",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{(TraitSymbol *) symbols["::.Number"].get()}
        ));
        quickLookup.emplace("::.Double", symbols["::.Double"].get());
        symbols.emplace("::.Bool", std::make_unique<ClassSymbol>(
                nullptr,
                "Bool",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{(TraitSymbol *) symbols["::.Number"].get()}
        ));
        quickLookup.emplace("::.Bool", symbols["::.Bool"].get());
        symbols.emplace("::.Char", std::make_unique<ClassSymbol>(
                nullptr,
                "Char",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{}
        ));
        quickLookup.emplace("::.Char", symbols["::.Char"].get());
        symbols.emplace("::.Array", std::make_unique<ClassSymbol>(
                nullptr,
                "Array",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | ValueType),
                std::vector<TraitSymbol *>{}
        ));
        quickLookup.emplace("::.Array", symbols["::.Array"].get());
        symbols.emplace("::.Trait", std::make_unique<ClassSymbol>(
                nullptr,
                "Trait",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | Final),
                std::vector<TraitSymbol *>{}
        ));
        quickLookup.emplace("::.Trait", symbols["::.Trait"].get());
        symbols.emplace("::.Class", std::make_unique<ClassSymbol>(
                nullptr,
                "Class",
                std::vector<ClassSymbol>(),
                nullptr,
                (Modifiers) (Static | Final),
                std::vector<TraitSymbol *>{((ClassSymbol *) symbols["::.Trait"].get())}
        ));
        quickLookup.emplace("::.Class", symbols["::.Class"].get());
        symbols.emplace("::.Function", std::make_unique<ClassSymbol>(
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
        quickLookup.emplace("::.Function", symbols["::.Function"].get());
    }

    std::vector<Symbol *> SymbolTable::lookup(std::string_view name) {
        std::vector<Symbol *> result;

        return std::vector<Symbol *>();
    }

    std::vector<Symbol *> SymbolTable::lookup(std::string_view name, Symbol *scope) {
        return std::vector<Symbol *>();
    }
} // Seserot