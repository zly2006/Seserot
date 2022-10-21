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
        // Global or builtin symbols, cached for fast lookup.
        // Note: entries in this map are available in all scopes.
        std::unordered_map<std::string, Symbol *> quickLookup;
        Scope *root;
    public:
        std::map<std::string, std::unique_ptr<Symbol>> symbols;
        std::vector<SymbolTable *> importedTables;

        explicit SymbolTable(Scope *root, bool = false);

        std::vector<Symbol *> lookup(std::string_view name);

        std::vector<Symbol *> lookup(std::string_view name, Symbol *scope);
    };
} // Seserot

#endif //SESEROT_GEN0_SYMBOL_TABLE_H
