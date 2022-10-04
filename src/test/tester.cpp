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
#include "../utils/sum_string.h"
#include <iostream>

using namespace Seserot;
using namespace llvm;

#define TEST(expr) if (!(expr)) { std::cout << "test for `" #expr "` failed\n"; return false; }

bool test(const std::string& what) {
    if (what == "params-matching") {
        MethodSymbol methodSymbol(nullptr, "test", Modifiers::None, {}, {}, nullptr);
        BuildIn buildIn;
        std::vector<size_t> expected = {0};
        std::cout << "Testing basic..." << std::endl;
        auto numberArg = VariableSymbol(nullptr, "test", nullptr, Modifiers::None, buildIn.numberTrait);
        methodSymbol.args.push_back(numberArg);
        TEST(methodSymbol.match({buildIn.numberTrait}) == expected)
        std::cout << "Testing generic..." << std::endl;
        methodSymbol.genericArgs.push_back({nullptr, "Test", {}, nullptr, Modifiers::None});
        VariableSymbol myGenericArg = {nullptr, "test", nullptr, Modifiers::None, &methodSymbol.genericArgs[0]};
        methodSymbol.args[0] = myGenericArg;
        TEST(methodSymbol.match({buildIn.numberTrait}) == expected)
        std::cout << "Testing vararg..." << std::endl;
        expected = {};
        ClassSymbol newClass = {nullptr, "Test", {}, nullptr, Modifiers::None};
        VariableSymbol newClassArg = {nullptr, "test", nullptr, Modifiers::Vararg, &newClass};
        methodSymbol.args[0] = newClassArg;
        TEST(methodSymbol.match({}) == expected)
        std::cout << "Testing all..." << std::endl;
        expected = { 0, 0, 1, 1};
        myGenericArg.modifiers = Modifiers::Vararg;
        methodSymbol.args.push_back(myGenericArg);
        TEST(methodSymbol.match({ &newClass, &newClass, buildIn.numberTrait, buildIn.stringClass}) == expected)
        return true;
    }
    if (what == "hello-world") {

        return true;
    }
    else {
        std::cout << "test " << what << " not found" << std::endl;
        return false;
    }
}