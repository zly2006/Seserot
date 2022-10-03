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

#include "tester.h"
#include "../Symbol.h"
#include "../Parser.h"
#include <iostream>

using namespace Seserot;
using namespace llvm;

#define TEST(expr) if (!(expr)) { std::cout << "test for `" #expr "` failed\n"; return false; }

bool test(const std::string& what) {
    if (what == "params-matching") {
        std::cout << "Testing params-matching..." << std::endl;
        MethodSymbol methodSymbol(nullptr, "test", Modifiers::None, {}, {}, nullptr);
        BuildIn buildIn;
        std::vector<size_t> expected = {0};
        // test basic
        methodSymbol.args.push_back(buildIn.numberTrait);
        TEST(methodSymbol.match({buildIn.numberTrait}) == expected)
        // test generic
        methodSymbol.genericArgs.push_back({nullptr, "Test", {}, nullptr, Modifiers::None});
        methodSymbol.args[0] = &methodSymbol.genericArgs[0];
        TEST(methodSymbol.match({buildIn.numberTrait}) == expected)
        expected = {};
        // test vararg
        ClassSymbol newClass = {nullptr, "Test", {}, nullptr, Modifiers::Vararg};
        methodSymbol.args[0] = &newClass;
        TEST(methodSymbol.match({}) == expected)
        std::cout << "Test passed." << std::endl;
        return true;
    }
    else {
        std::cout << "test " << what << " not found" << std::endl;
        return false;
    }
}