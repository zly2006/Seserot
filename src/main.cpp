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

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <llvm/IR/LLVMContext.h>
#include "Lexer.h"
#include "Parser.h"
#include "utils/ByteOrder.h"
#include "generated/version.h"
#include "test/tester.h"

std::optional<int> build(const std::filesystem::path& path) {
    std::filesystem::directory_iterator iterator(path);
    if (!iterator->exists()) {
        return {};
    }
    for (auto &item: iterator) {
        std::filesystem::current_path();
    }
    return 0;
}

int main(int _argc, char **_argv) {
    static_assert(
            getEndianOrder() != hl_endianness::HL_PDP_ENDIAN &&
            getEndianOrder() != hl_endianness::HL_UNKNOWN_ENDIAN);
    static_assert(sizeof(size_t) == 8);
    static_assert(sizeof(short) == 2);
    static_assert(sizeof(int) == 4);
    static_assert(sizeof(long) == 8);
    static_assert(sizeof(char) == 1);


    llvm::LLVMContext thisContext;
    llvm::IRBuilder<> Builder(thisContext);
    std::unique_ptr<llvm::Module> thisModule;
    std::map<std::string, llvm::Value *> NamedValues;
    std::vector<std::string> args;
    std::cout << "Welcome to Seserot!\nThis program is free software under the GNU General Public License.\n";
    for (int i = 1; i < _argc; ++i) {
        args.emplace_back(_argv[i]);
    }

    for (int i = 0; i < args.size(); ++i) {
        auto& c = args[i];
        if (c == "-h" || c == "--help") {
            std::cout << R"(
usage: seserot [-h | -v | --llvm | --build] [options] <source file | project file>...

Commands:
    -h | -help      Show help page.
    -v | --version  Show version.
    --about         Show about message.
    -b | --build    Generate executable.

Options:
    --llvm          Don't build, just generate LLVM IR.
)";
        }
        else if (c == "-v" || c == "--version") {
            std::cout << "Seserot " << SESEROT_VERSION_MAJOR << "." << SESEROT_VERSION_MINOR << "." <<
                      SESEROT_VERSION_PATCH << "\n";
        }
        else if (c == "--about") {
            std::cout << R"(
Copyright (C) 2022 zly2006

Source: <https://github.com/zly2006/Seserot>
Homepage: <https://seserot.se> //我好像要谁捐一个/doge
)";
        }
        else if (c == "--dev-test") {
            i++;
            std::cout << "Testing " << args[i] << "...\n";
            if (test(args[i])) {
                std::cout << "test " << args[i] << " passed" << std::endl;
            }
            else {
                std::cout << "test " << args[i] << " failed" << std::endl;
                return 1;
            }
        }
    }
    return 0;
}
