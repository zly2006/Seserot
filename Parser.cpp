//
// Created by 赵李言 on 2022/8/11.
//

#include "Parser.h"
#include "Symbol.h"
#include "BuildIn.h"

#include <utility>
#include <stack>

namespace Seserot {
    void Parser::reset() {
        for (const auto &item: namespaces) {
            delete item.second;
        }
        for (const auto &item: classes) {
            delete item.second;
        }
        for (const auto &item: methods) {
            delete item.second;
        }
        namespaces.clear();
        classes.clear();
        methods.clear();
        auto loadClass = [&](const ClassSymbol *p) {
            classes.emplace(p->name, p);
        };
        for (auto &item: root.children) {
            delete item;
        }
        loadClass(buildIn.voidClass);
        loadClass(buildIn.doubleClass);
        loadClass(buildIn.intClass);
        loadClass(buildIn.floatClass);
        loadClass(buildIn.longClass);
        loadClass(buildIn.functionClass);
    }

    void Parser::scan() {
        reset();
        Scope* currentScope = &root;
        SymbolWithChildren* rootNamespace = new NamespaceSymbol(currentScope, "<root>");
        namespaces.emplace("",(NamespaceSymbol*)rootNamespace);
        std::stack<SymbolWithChildren*>currentFatherSymbol;
        currentFatherSymbol.emplace(rootNamespace);
        // init finish

        auto newScope = [&](Token& thisToken) {
            currentScope = &currentScope->newChildren(thisToken.start,SourcePosition(0,0));
        };
        auto endScope = [&](Token& thisToken) {
            currentScope->stop = thisToken.start;
            if (currentFatherSymbol.top()->childScope == currentScope) {
                currentFatherSymbol.pop();
            }
            if (currentScope->father == nullptr) {
                errorTable.interrupt();
            }
            currentScope = currentScope->father;
        };
        for (int i = 0; i < tokens.size(); ++i) {
            if (tokens[i].type == Token::Operator) {
                if (tokens[i].content == "{") {
                    newScope(tokens[i]);
                } else if (tokens[i].content == "}") {
                    endScope(tokens[i]);
                }
            }
            if (tokens[i].content == "namespace") {
                if (i + 1 >= tokens.size() || tokens[i + 1].type != Token::Name) {
                    errorTable.errors.emplace_back(tokens[i].stop,2011,"namespace not found.");
                    errorTable.interrupt("scanning namespaces");
                }
                i++;
                if (currentFatherSymbol.top()->type != Symbol::Namespace) {
                    errorTable.errors.emplace_back(tokens[i].start,2501,
                                                          "Namespace can only be defined in namespace scope.");
                    errorTable.interrupt("scanning namespaces");
                }
                std::string name = tokens[i].content;
                if (currentFatherSymbol.top() != rootNamespace) {
                    name = currentFatherSymbol.top()->name + "." + name;
                }
                if (namespaces.count(name)) {
                    errorTable.errors.emplace_back(tokens[i].stop, 2502, "namespace definition already exists.");
                    errorTable.interrupt("scanning namespaces");
                }
                namespaces.emplace(name, new NamespaceSymbol(currentScope, name));
            }
            if (tokens[i].content == "class") {
                if (i + 1 >= tokens.size() || tokens[i + 1].type != Token::Name) {
                    errorTable.errors.emplace_back(tokens[i].stop, 2012, "class name not found.");
                    errorTable.interrupt("scanning classes");
                }
                i++;
                std::string name = tokens[i].content;
                if (currentFatherSymbol.top() != rootNamespace) {
                    name = currentFatherSymbol.top()->name + "." + name;
                }
                if (classes.count(name)) {
                    errorTable.errors.emplace_back(tokens[i].start, 2503, "class definition already exists.");
                    errorTable.interrupt("scanning classes");
                }
                auto* symbol = new ClassSymbol(currentScope, name, {}, nullptr, None);
                classes.emplace(name,symbol);
                while (true) {
                    i++;
                    if (tokens[i].content == "{" && tokens[i].type == Token::Operator) {
                        newScope(tokens[i]);
                        symbol->childScope = currentScope;
                        currentFatherSymbol.push(symbol);
                        break;
                    }
                }
            }
            if (tokens[i].content == "function") {
                if (i + 1 >= tokens.size() || tokens[i + 1].type != Token::Name) {
                    errorTable.errors.emplace_back(tokens[i].stop,2013,"method name not found.");
                    errorTable.interrupt("scanning methods");
                }
                i++;
                std::string name = tokens[i].content;
                if (currentFatherSymbol.top() != rootNamespace) {
                    name = currentFatherSymbol.top()->name + "." + name;
                }
                if (methods.count(name)) {
                    errorTable.errors.emplace_back(tokens[i].start, 2504, "method definition already exists.");
                    errorTable.interrupt("scanning methods");
                }
                tokens[i].parsed = Token::Statement;
                if (i != tokens.size() - 1) {

                    if (tokens[i].type == Token::Operator && tokens.operator[](i).content == "<") {
                        //todo
                    }
                }
                auto modifiers = parseModifiers(tokens.begin() + i);

                methods.emplace(name, nullptr);//todo
            }
        }
    }

    Parser::Parser(std::vector<Token> tokens, ErrorTable &errorTable)
            : tokens(std::move(tokens)), errorTable(errorTable) {}

    std::vector<std::string> Parser::parseModifiers(std::vector<Token>::iterator it) {
        std::vector<std::string> ret;
        if (it == tokens.begin()) return ret;
        auto first = it - 1;
        while (modifiers.count(first->content)) {
            if (first->type == Token::Name) {
                ret.push_back(first->content);
            }
            first->parsed = Token::Statement;
            if (first != tokens.begin())
                break;
            first--;
        }
        return ret;
    }
} // Seserot