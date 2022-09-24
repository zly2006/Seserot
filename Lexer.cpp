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

#include "Lexer.h"
#include <regex>

namespace Seserot {
    Lexer::State Lexer::parseNextChar() {
        if (cursor> fileContent.length()) {
            errorTable.interrupt("Cursor is after the end.");
            return FatalError;
        }
        if (state == String) {
            if (fileContent[cursor] == '\\') {
                moveCursor(2);
                return String;
            }
            else if (fileContent[cursor] == '\"') {
                tokens.push_back(Token{
                    startPosition, position,
                    Token::Literal, parseLiteral(fileContent.substr(start, cursor - start))
                });
                moveCursor();
                return Ready;
            }
            else if (fileContent[cursor] == '\n') {
                errorTable.interrupt("Unexpected new line in string.");
                return FatalError;
            } else {
                moveCursor();
                return String;
            }
        }
        else if (state == MultiLineString) {
            /*todo:
             * 1. 多行字符串的识别和终止存在问题
             * 2. 转义字符问题
             */
            if (fileContent.length() > cursor + 2
                && fileContent[cursor + 0] == '\"'
                && fileContent[cursor + 1] == '\"'
                && fileContent[cursor + 2] == '\"') {
                tokens.push_back(Token{
                        startPosition, position,
                        Token::Literal, parseLiteral(fileContent.substr(start, cursor - start))
                });
                moveCursor(3);
                return Ready;
            }
            else {
                moveCursor();
                return MultiLineString;
            }
        }
        else if (state == Comment) {
            moveCursor();
            if (fileContent[cursor] == '\n') {
                return Ready;
            }
            else {
                return Comment;
            }
        }
        else if (state == MultiLineComment) {
            if (fileContent[cursor] == '*'
                && fileContent.length() > cursor + 1
                && fileContent[cursor + 1] == '/') {
                moveCursor(2);
                return Ready;
            } else {
                moveCursor();
                return MultiLineComment;
            }
        }
        else if (state == Number) {
            if (isdigit(fileContent[cursor])) {
                moveCursor();
                return Number;
            } else if (numberComponents.count(fileContent[cursor])
                && fileContent.length() > cursor + 1
                && isdigit(fileContent[cursor + 1])) {
                /* strict
                 * allowed:
                 * 1.0
                 * 0.0
                 * disallowed:
                 * .1
                 * 1,
                 */
                moveCursor(2); // skip the digit char after '.'
                return Number;
            }
            else {
                tokens.push_back(Token{
                    startPosition, position,
                    Token::Type::Number, std::string (fileContent.substr(start, cursor - start))
                });
                return Ready;
            }
        }
        else if (state == Identifier) {
            if (isalpha(fileContent[cursor])
                || fileContent[cursor] == '_'
                || isdigit(fileContent[cursor])) {
                moveCursor();
                return Identifier;
            }
            else {
                tokens.push_back(Token{
                        startPosition, position,
                        Token::Type::Name, std::string(fileContent.substr(start, cursor - start))
                });
                return Ready;
            }
        }
        else if (state == WideIdentifier) {
            if (fileContent[cursor] == '`') {
                tokens.push_back(Token{
                        startPosition, position,
                        Token::Type::Name, std::string (fileContent.substr(start, cursor - start))
                });
                moveCursor();
                return Ready;
            } else {
                moveCursor();
                return WideIdentifier;
            }
        }
        else if (state == Ready) {
            if (fileContent[cursor] == '\t'
                   || fileContent[cursor] == ' ') {
                moveCursor();
                return Ready;
            } else if (fileContent[cursor] == '\n') {
                syncStart();
                moveCursor();
                tokens.push_back(Token{
                    startPosition,position,
                    Token::NewLine
                });
                return Ready;
            }
            if (fileContent[cursor] == '\"') {
                if (fileContent.length() > cursor + 2
                    && fileContent[cursor + 1] == '\"'
                    && fileContent[cursor + 2] == '\"') {
                    moveCursor(3);
                    syncStart();
                    return MultiLineString;
                } else {
                    moveCursor();
                    syncStart();
                    return String;
                }
            }
            else if (fileContent[cursor] == '/' && fileContent.length() > cursor + 1) {
                if (fileContent[cursor + 1] == '/') {
                    moveCursor(2);
                    return Comment;
                } else if (fileContent[cursor + 1] == '*') {
                    moveCursor(2);
                    return MultiLineComment;
                }
            }
            else if (isdigit(fileContent[cursor])) {
                syncStart();
                moveCursor();
                return Number;
            }
            else if (isalpha(fileContent[cursor]) || fileContent[cursor] == '_') {
                syncStart();
                moveCursor();
                return Identifier;
            }
            else if (fileContent[cursor] == '`') {
                moveCursor();
                syncStart();
                return WideIdentifier;
            }
            else {
                for (const auto& op: operators) {
                    if (fileContent.length() >= op.length() + cursor) {
                        if (fileContent.substr(cursor, op.length()) == op) {
                            syncStart();
                            moveCursor(op.length());
                            tokens.push_back(Token{
                                startPosition, position,
                                Token::Operator, std::string(fileContent.substr(start, cursor - start))
                            });
                            return Ready;
                        }
                    }
                }
            }
        }
        errorTable.interrupt("Unexpected state.");
        return FatalError;
    }

    void Lexer::syncStart() {
        startPosition = position;
        start = cursor;
    }

    std::string Lexer::parseLiteral(std::string_view content) {
        std::string string;
        string.reserve(content.length());
        for (size_t i = 0; i < content.length(); i++) {
            if (content[i] == '\\') {
                i++;
                if (i == content.length()) {
                    errorTable.interrupt();
                }
                switch (content[i]) {
                    case 'a':
                        string.push_back('\a');
                    case 'n':
                        string.push_back('\n');
                    case 't':
                        string.push_back('\t');
                    case 'r':
                        string.push_back('\r');
                    case 'f':
                        string.push_back('\f');
                    case 'v':
                        string.push_back('\v');
                    case '0':
                        string.push_back('\0');
                    case '\'':
                        string.push_back('\'');
                    case '\"':
                        string.push_back('\"');
                    case '\\':
                        string.push_back('\\');
                    case 'x': {
                        if (i + 2 < content.length()) {
                            char a[2];
                            a[0] = content[i + 1];
                            a[1] = content[i + 2];
                            for (char &j: a) {
                                if (j <= '9' && j >= '0')
                                    j -= '0';
                                else if (j <= 'f' && j >= 'a')
                                    j -= 'a';
                                else if (j <= 'F' && j >= 'A')
                                    j -= 'A';
                                else errorTable.interrupt();
                            }
                            unsigned char c = a[0] << 4 | a[1];
                            string.push_back(c);
                        }
                        else {
                            errorTable.interrupt("parse literal: HEX");
                        }
                    }
                    case 'u': {
                        if (i + 4 < content.length()) {
                            errorTable.interrupt("TODO");
                        }
                        else {
                            errorTable.interrupt("parse literal: Unicode");
                        }
                    }
                }
            }
            else {
                string.push_back(content[i]);
            }
        }
        return string;
    }

    void Lexer::moveCursor(size_t bias) {
        for (size_t i = 0; i < bias; ++i) {
            if (fileContent[cursor] == '\n') {
                position.line++;
                position.column = 0;
            } else position.column++;
            cursor++;
        }
    }

    void Lexer::parse() {
        while (cursor != fileContent.length()) {
            state = parseNextChar();
        }
        if (state != Ready) {
            errorTable.interrupt("End state is not ready.");
        }
    }
} // Seserot