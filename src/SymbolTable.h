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
    class SymbolTable;

    extern SymbolTable builtinTable;

    class SymbolTable {
    private:
        Scope *root;
    public:
        /**
         * @brief Construct a new Symbol Table object. If root is not nullptr,
         * ths created table will import builtin symbols.
         */
        explicit SymbolTable(Scope *root = nullptr);

        std::map<std::string, std::unique_ptr<Symbol>> symbols;

        std::vector<SymbolTable *> importedTables;

        std::vector<Symbol *> lookup(std::string_view name);

        std::vector<Symbol *> lookup(std::string_view name, Scope *scope);

        bool emplace(std::unique_ptr<Symbol> symbol);

        static TraitSymbol *Number;
        static ClassSymbol *String;
        static ClassSymbol *Void;
        static ClassSymbol *Int;
        static ClassSymbol *Long;
        static ClassSymbol *Short;
        static ClassSymbol *Float;
        static ClassSymbol *Double;
        static ClassSymbol *Char;
        static ClassSymbol *Boolean;
        static ClassSymbol *Byte;
        static ClassSymbol *Object;
        static ClassSymbol *Array;
        static ClassSymbol *Trait;
        static ClassSymbol *Class;
        static ClassSymbol *Enum;
        static ClassSymbol *Function;
    };
} // Seserot

#endif //SESEROT_GEN0_SYMBOL_TABLE_H
