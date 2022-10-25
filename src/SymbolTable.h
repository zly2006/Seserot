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

#ifndef SESEROT_GEN0_SYMBOL_TABLE_H
#define SESEROT_GEN0_SYMBOL_TABLE_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <string_view>
#include "Symbol.h"


namespace Seserot {
    class SymbolTable {
    private:
        Scope *root;

        NamespaceSymbol *currentNamespace;
    public:
        /**
         * @brief Construct a new Symbol Table object. If root is not nullptr,
         * ths created table will import builtin symbols.
         */
        explicit SymbolTable(Scope *root = nullptr, NamespaceSymbol *namespaceSymbol = nullptr);

        std::map<std::string, std::unique_ptr<Symbol>> symbols;

        std::vector<SymbolTable *> importedTables;

        std::vector<Symbol *> lookup(std::string_view name);

        std::vector<Symbol *> lookup(std::string_view name, Scope *scope);

        bool emplace(std::unique_ptr<Symbol> symbol);
    };

    SymbolTable &getSymbolTable(const std::string &name);

    TraitSymbol *getCommonBaseClass(TraitSymbol *a, TraitSymbol *b);

    namespace BuiltinSymbols {
        void init();

        extern SymbolTable builtinTable;
        extern TraitSymbol *Number;
        extern ClassSymbol *String;
        extern ClassSymbol *Void;
        extern ClassSymbol *Int;
        extern ClassSymbol *Long;
        extern ClassSymbol *Short;
        extern ClassSymbol *Float;
        extern ClassSymbol *Double;
        extern ClassSymbol *Char;
        extern ClassSymbol *Boolean;
        extern ClassSymbol *Byte;
        extern ClassSymbol *Object;
        extern ClassSymbol *Array;
        extern ClassSymbol *Trait;
        extern ClassSymbol *Class;
        extern ClassSymbol *Enum;
        extern ClassSymbol *Function;
    }
} // Seserot

#endif //SESEROT_GEN0_SYMBOL_TABLE_H
