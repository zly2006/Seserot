//
// Created by 赵李言 on 2022/8/11.
//

#ifndef SESEROT_GEN0_PARSER_H
#define SESEROT_GEN0_PARSER_H
#include <vector>
#include "BasicStructures.h"
#include "ErrorTable.h"
#include "Symbol.h"
#include "BuildIn.h"
#include <map>
#include <unordered_map>

namespace Seserot {
    struct Expression{
        enum Type{
            VirtualCall,
            Call,
            Read,
            Write,
        };
        Type type;
        Symbol* receiver;
        std::vector<Expression*> arguments;
        ClassSymbol* returnType;
    };

    class Parser {
    public:
        BuildIn buildIn;
        ErrorTable& errorTable;
        Parser(std::vector<Token> tokens, ErrorTable &errorTable);
        FileScope root;
        std::vector<Token> tokens;
        std::map<std::string, const NamespaceSymbol*> namespaces;
        std::map<std::string, const ClassSymbol*> classes;
        std::map<std::string, const Symbol*> methods;
        void reset();
        void scan();
        std::vector<Token> parseModifiers(std::vector<Token>::iterator it);
    };

} // Seserot

#endif //SESEROT_GEN0_PARSER_H
