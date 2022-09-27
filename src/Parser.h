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
#include <any>
#include <optional>
#include "AbstractSyntaxTreeNode.h"

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
        friend int ::main(int, char**);
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

        size_t generateStack(MethodSymbol*);
        void parseReference();
        /*void processGeneric();
        void generateTypeVar();
        void parseFunctionAST();*/
        Modifiers parseModifiers(std::vector<Token>::iterator it, Modifiers = None);
    private:
        Token &read(size_t&);
        std::vector<Symbol*> searchSymbol(Symbol::Type, const std::string&, Scope*);
        static ClassSymbol* currentClassSymbol(Symbol*);
        static MethodSymbol* currentMethodSymbol(Symbol*);
        AbstractSyntaxTreeNode *parseExpression(token_iter &tokenIter, char untilBracket = 0);
        static size_t string2FitNumber(const std::string &str, char *ptr, bool = false);
        Token &expectIdentifier(size_t &pos);
        Token &expectOperator(size_t &pos);
        bool expectIdentifier(size_t &pos, const std::string& content);

        template<class T>
        std::optional<T> convertToNumber(const std::string& str);

        std::map<Token*, Symbol*> reference;
        std::map<Token*, Scope*> token2scope;
        std::vector<MethodSymbol> specializedGenericMethod;
        std::vector<ClassSymbol> specializedGenericClass;
        const std::set<std::string> modifiers {
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
        const std::set<std::string> modifierAccessibility {
                "public",
                "protected",
                "internal",
                "private"
        };
        const std::set<std::string> modifierMutable {
                "mutable",
                "immutable"
        };
    };

} // Seserot

#endif //SESEROT_GEN0_PARSER_H
