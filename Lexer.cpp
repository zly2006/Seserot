//
// Created by 赵李言 on 2022/8/10.
//

#include "Lexer.h"
#include <regex>

namespace Seserot {
    Lexer::State Lexer::parseNextChar() {
        if (cursor> fileContent.length()) {
            errorTable.interrupt("Cursor is after the end.");
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
            if (fileContent.length() > cursor + 2
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
                    Token::Type::Name, std::string (fileContent.substr(start, cursor - start))
                });
                return Ready;
            }
        }
        else if (state == Identifier) {
            if (isalpha(fileContent[cursor]) || fileContent[cursor] == '_') {
                moveCursor();
                return Identifier;
            } else {
                tokens.push_back(Token{
                        startPosition, position,
                        Token::Type::Name, std::string (fileContent.substr(start, cursor - start))
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
                   || fileContent[cursor] == ' '
                   || fileContent[cursor] == '\n'
                    ) {
                moveCursor();
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
                i ++;
                if (i == content.length()) {
                    errorTable.interrupt();
                }
                switch (content[i]) {
                    case 'a':
                        string += '\a';
                    case 'n':
                        string += '\n';
                    case 't':
                        string += '\t';
                    case 'r':
                        string += '\r';
                    case 'f':
                        string += '\f';
                    case 'v':
                        string += '\v';
                    case '0':
                        string += '\0';
                    case '\'':
                        string += '\'';
                    case '\"':
                        string += '\"';
                    case '\\':
                        string += '\\';
                    case 'x': {
                        if (i + 2 < content.length()) {
                            char a[2];
                            a[0] = content[i+1];
                            a[1] = content[i+2];
                            for (int j = 0; j < 2; ++j) {
                                if (a[j] <= '9' && a[j] >= '0') {
                                    a[j] = a[j] - '0';
                                } else if (a[j] <= 'f' && a[j] >= 'a') {
                                    a[j] = a[j] - 'a';
                                } else if (a[j] <= 'F' && a[j] >= 'A') {
                                    a[j] = a[j] - 'A';
                                } else errorTable.interrupt();
                            }
                        } else {
                            errorTable.interrupt("parse literal: HEX");
                        }
                    }
                    case 'u': {
                        if (i + 4 < content.length()) {
                            errorTable.interrupt("TODO");
                        } else {
                            errorTable.interrupt("parse literal: Unicode");
                        }
                    }
                }
            } else {
                string += content[i];
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