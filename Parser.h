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
#include <set>

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
    private:
        using token_iter = std::vector<Token>::iterator;
    public:
        BuildIn buildIn;
        ErrorTable& errorTable;
        Parser(std::vector<Token> tokens, ErrorTable &errorTable);
        FileScope root;
        std::vector<Token> tokens;
        std::map<std::string, NamespaceSymbol*> namespaces;
        std::map<std::string, ClassSymbol*> classes;
        std::multimap<std::string, Symbol*> methods;
        std::vector<VariableSymbol*> variables;
        std::vector<PropertySymbol*> properties;
        void parse();
        void reset();
        void scan();
        void parseReference();
        /*void processGeneric();
        void generateTypeVar();
        void parseFunctionAST();*/
        Modifiers parseModifiers(std::vector<Token>::iterator it, Modifiers = None);
    private:
        Token &read(size_t&);
        Symbol* searchSymbol(Symbol::Type, std::string, Scope*);
        static ClassSymbol* currentClassSymbol(Symbol*);
        static MethodSymbol* currentMethodSymbol(Symbol*);

        std::map<Token*, Symbol*> reference;
        std::map<Token*, Scope*> token2scope;
        std::vector<MethodSymbol> specializedGenericMethod;
        std::vector<ClassSymbol> specializedGenericClass;
        std::set<std::string> modifiers {
                "final",
                "static",
                "public",
                "protected",
                "internal",
                "private",
                "mutable",
                "immutable",
                "inner",
                "partial",
        };
        std::set<std::string> modifierAccessibility {
                "public",
                "protected",
                "internal",
                "private"
        };
        std::set<std::string> modifierMutable {
                "mutable",
                "immutable"
        };

    };

} // Seserot

#endif //SESEROT_GEN0_PARSER_H
