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

#ifndef SESEROT_GEN0_LEXER_H
#define SESEROT_GEN0_LEXER_H

#include <string>
#include <set>
#include "BasicStructures.h"
#include "ErrorTable.h"

namespace Seserot {

    class Lexer {
    public:

        std::vector<Token> tokens;

        Lexer(ErrorTable &errorTable, std::string_view fileContent) :
                errorTable(errorTable),
                fileContent(fileContent) {
            cursor = 0;
            start = 0;
        }

        enum State {
            Ready,
            String,
            MultiLineString,
            FatalError,
            Identifier,
            WideIdentifier,
            Comment,
            MultiLineComment,
            Number,
            Character,
            Operator,
        };
        State state = Ready;
        std::string_view fileContent;
        size_t cursor;
        size_t start;
        ErrorTable &errorTable;
        SourcePosition position{1, 0};
        SourcePosition startPosition{1, 0};

        void parse();

    private:
        const std::vector<std::string> operators = {
                "<<=", ">>=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
                "&&", "||", "!=", "==", "..", "=>", "->", "--", "++",
                "+", "-", "*", "/", "%", ".", "<", ">", "!", "&", "|", "~", "=", ",", ":", "?", ";",
                "(", ")", "{", "}", "[", "]",
                "<<", ">>"//generic type arg
        };
        const std::set<char> numberComponents = {
                '.', 'e', 'E', 'l', 'L', 'u', 'U', // dot & suffix
                'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F' // Hex
        };

        State parseNextChar();

        std::string parseLiteral(std::string_view);

        void syncStart();

        void moveCursor(size_t = 1);
    };

} // Seserot

#endif //SESEROT_GEN0_LEXER_H
