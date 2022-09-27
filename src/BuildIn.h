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

#ifndef SESEROT_GEN0_BUILD_IN_H
#define SESEROT_GEN0_BUILD_IN_H
#include "Symbol.h"
namespace Seserot {

    class BuildIn {
    public:
        ClassSymbol *const voidClass = new ClassSymbol(
                nullptr,
                "Void",
                {},
                nullptr, Static | ValueType);
        ClassSymbol *const intClass = new ClassSymbol(
                nullptr,
                "Int",
                {},
                nullptr, Static | ValueType);
        ClassSymbol *const longClass = new ClassSymbol(
                nullptr,
                "Long",
                {},
                nullptr, Static | ValueType);
        ClassSymbol *const floatClass = new ClassSymbol(
                nullptr,
                "Float",
                {},
                nullptr, Static | ValueType);
        ClassSymbol *const doubleClass = new ClassSymbol(
                nullptr,
                "Double",
                {},
                nullptr, Static | ValueType);
        ClassSymbol *const stringClass = new ClassSymbol(
                nullptr,
                "String",
                {},
                nullptr, Static);
        ClassSymbol *const classClass = new ClassSymbol(
                nullptr,
                "Class",
                {},
                nullptr, Static);
        ClassSymbol *const functionClass = new ClassSymbol(
                nullptr,
                "Function",
                {
                        ClassSymbol(nullptr, "P", {}, nullptr, None),
                        ClassSymbol(nullptr, "T", {}, nullptr, None),
                        ClassSymbol(nullptr, "...", {}, nullptr, None),
                },
                nullptr, Static | ValueType);

        TraitSymbol *const numberTrait = new TraitSymbol(
                nullptr,
                "Number",
                (Modifiers) (Static | ValueType)
        );
    };

} // Seserot

#endif //SESEROT_GEN0_BUILD_IN_H
