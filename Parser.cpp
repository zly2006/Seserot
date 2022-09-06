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
        auto loadClass = [&](ClassSymbol *p) {
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
                if (!namespaces.contains(name)) {
                    namespaces.emplace(name, new NamespaceSymbol(currentScope, name));
                }
                currentFatherSymbol.push(namespaces[name]);
            }
            if (tokens[i].content == "class") {
                if (i + 1 >= tokens.size() || tokens[i + 1].type != Token::Name) {
                    //errorTable.errors.emplace_back(tokens[i].stop, 2012, "class name not found.");
                    //errorTable.interrupt("scanning classes");
                }
                auto mod = parseModifiers(tokens.begin() + i);
                i++;
                std::string name = tokens[i].content;
                if (currentFatherSymbol.top() != rootNamespace) {
                    name = currentFatherSymbol.top()->name + "." + name;
                }
                if (classes.count(name) && !(mod & Partial)) {
                    errorTable.errors.emplace_back(tokens[i].start, 2503, "class definition already exists.");
                    errorTable.interrupt("scanning classes");
                }
                ClassSymbol* symbol;
                if (classes.contains(name))
                    symbol = classes[name];
                else {
                    symbol = new ClassSymbol(currentScope, name, {}, nullptr, None);
                    classes.emplace(name, symbol);
                }
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
                Modifiers mod = parseModifiers(tokens.begin() + i);
                i++;
                std::string name = tokens[i].content;
                if (currentFatherSymbol.top() != rootNamespace) {
                    name = currentFatherSymbol.top()->name + "." + name;
                }
                if (methods.count(name)) {
                    errorTable.errors.emplace_back(tokens[i].start, 2504, "method definition already exists.");
                    errorTable.interrupt("scanning methods");
                }
                auto* methodSymbol = new MethodSymbol(nullptr, name, None,
                                                              {}, {});
                tokens[i].parsed = Token::Statement;
                if (i != tokens.size() - 1) {
                    if (tokens[i].type == Token::Operator && tokens[i].content == "<") {
                        //todo
                    }
                }

                methods.emplace(name, methodSymbol);//todo
            }
        }
    }

    Parser::Parser(std::vector<Token> tokens, ErrorTable &errorTable)
            : tokens(std::move(tokens)), errorTable(errorTable) {}

    Modifiers Parser::parseModifiers(std::vector<Token>::iterator it, Modifiers def) {
        std::set<std::string_view> ret;
        if (it == tokens.begin()) return None;
        auto first = it - 1;
        while (modifiers.count(first->content)) {
            if (first->type == Token::Name) {
                if (ret.contains(first->content)) {
                    errorTable.errors.emplace_back(first->start, 0, "");
                    errorTable.interrupt("parsing modifiers.");
                }
                ret.insert(first->content);
            }
            first->parsed = Token::Statement;
            if (first != tokens.begin())
                break;
            first--;
        }
        unsigned int m = def;
        bool accessibilityParsed = false;
        bool mutableParsed = false;
        for (auto& mo : ret) {
            if (mo == "public" || mo == "private" || mo == "internal" || mo == "protected") {
                if (accessibilityParsed) {
                    errorTable.errors.emplace_back(it->start, 0, "");//todo
                    errorTable.interrupt();
                }
                accessibilityParsed = true;
                if (mo == "public") {

                }
                else if (mo == "private") {
                    m |= PrivateInternal;
                    m |= PrivateProtected;
                }
                else if (mo == "protected") {
                    m |= PrivateProtected;
                }
                else if (mo == "internal") {
                    m |= PrivateInternal;
                }
            }
            else if (mo == "mutable" || mo == "immutable") {
                if (mutableParsed) {
                    errorTable.errors.emplace_back(it->start, 0, "");//todo
                    errorTable.interrupt();
                }
                mutableParsed = true;
                if (mo == "mutable") {
                    m |= Mutable;
                }
            }
        }
        return (Modifiers)m;
    }
} // Seserot