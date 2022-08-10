//
// Created by 赵李言 on 2022/8/10.
//

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
            Operator,
        };
        State state = Ready;
        std::string_view fileContent;
        size_t cursor;
        size_t start;
        ErrorTable& errorTable;
        SourcePosition position{1,0};
        SourcePosition startPosition{1,0};
        void parse();
    private:
        const std::vector<std::string> operators = {
                "<<=", ">>=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
                "&&", "||", "!=", "==", "..", "=>", "->", "--", "++",
                "+", "-", "*", "/", "%", ".", "<", ">", "!", "&", "|", "~", "=", ",", ":", "?",
                "(", ")", "{", "}", "[", "]",
                "<<", ">>"//generic type arg
        };
        const std::set<char> numberComponents = {
                '.','e','E','l','L','u','U'
        };
        State parseNextChar();
        std::string parseLiteral(std::string_view);
        void syncStart();
        void moveCursor(size_t = 1);
    };

} // Seserot

#endif //SESEROT_GEN0_LEXER_H
