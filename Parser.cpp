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
            if (currentScope == nullptr) {
                errorTable.errors.emplace_back(thisToken.start, 0, "unexpected \'}\'");//todo
                errorTable.interrupt();
            }
            currentScope->stop = thisToken.start;
            if (currentFatherSymbol.top()->childScope == currentScope) {
                currentFatherSymbol.pop();
            }
            if (currentScope->father == nullptr) {
                errorTable.interrupt();
            }
            currentScope = currentScope->father;
        };
        for (size_t i = 0; i < tokens.size(); ++i) {
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
                    i++;//todo
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
                    currentFatherSymbol.top()->name + "." + name;
                }
                if (methods.count(name)) {
                    errorTable.errors.emplace_back(tokens[i].start, 2504, "method definition already exists.");
                    errorTable.interrupt("scanning methods");
                }
                auto* methodSymbol = new MethodSymbol(nullptr, name, mod,
                                                              {}, {});
                tokens[i].parsed = Token::Statement;
                ClassSymbol *father = currentClassSymbol(currentFatherSymbol.top());
                methodSymbol->father = father;
                if (father != nullptr && father->modifiers & Static) {
                    if (!(methodSymbol->modifiers & Static)) {
                        errorTable.errors.emplace_back(tokens[i].start, 0, "");//todo
                    }
                }
                if (i != tokens.size() - 1) {
                    if (tokens[i].type == Token::Operator && tokens[i].content == "<") {
                        Token &genericName = read(i);
                        if (genericName.type != Token::Name) {
                            errorTable.errors.emplace_back(tokens[i].start, 0, "");//todo
                            errorTable.interrupt();
                        }
                        methodSymbol->genericArgs.push_back
                        (ClassSymbol(nullptr, genericName.content, {}, nullptr, 0));
                    }
                }
                Token& args_optional = read(i);
                if (args_optional.type == Token::Operator && args_optional.content == "(") {
                    //todo
                    while (read(i).content != ")");
                }
                Token& where_optional = read(i);
                if (where_optional.type == Token::Name && where_optional.content == "where") {
                    //todo
                }
                else if (where_optional.type == Token::Operator && where_optional.content == "{") {
                    newScope(tokens[i]);
                    methodSymbol->childScope = currentScope;
                    currentFatherSymbol.push(methodSymbol);
                    for (auto &item: methodSymbol->genericArgs) {
                        item.father = methodSymbol;
                        item.scope = currentScope;
                    }
                }
                methods.emplace(name, methodSymbol);
            }
        }
        if (currentScope != &root) {
            errorTable.errors.emplace_back(tokens.back().stop, 0, "scope is not closed");
        }
        if (!errorTable.errors.empty()) {
            errorTable.interrupt("scanning failed.");
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
                    errorTable.errors.emplace_back(first->start, 2505, "repeating modifier.");
                }
                ret.insert(first->content);
            } else if (first->type != Token::NewLine) {
                // other
                break;
            }
            first->parsed = Token::Statement;
            if (first == tokens.begin())
                break;
            first--;
        }
        unsigned int m = def;
        bool accessibilityParsed = false;
        bool mutableParsed = false;
        for (auto& mo : ret) {
            if (mo == "public" || mo == "private" || mo == "internal" || mo == "protected") {
                if (accessibilityParsed) {
                    errorTable.errors.emplace_back(it->start, 2505, "conflict accessibility modifier.");
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
                    errorTable.errors.emplace_back(it->start, 2505, "conflict mutable modifier.");
                }
                mutableParsed = true;
                if (mo == "mutable") {
                    m |= Mutable;
                }
            }
            else if (mo == "static") {
                m |= Static;
            }
            else if (mo == "final") {
                m |= Final;
            }
            else if (mo == "abstract") {
                m |= Abstract;
            }
            else if (mo == "async") {
                m |= Async;
            }
            else if (mo == "readonly") {
                m |= Readonly;
            }
            else if (mo == "operator") {
                m |= Operator;
            }
            else if (mo == "value") {
                m |= ValueType;
            }
            else if (mo == "inner") {
                m |= Inner;
            }
        }
        if ((m & Final) && (m & Static)) {
            errorTable.errors.emplace_back(it->start, 2505, "static member cannot be final.");
        }
        return (Modifiers)m;
    }

    Token &Parser::read(size_t& i) {
        do {
            i++;
            if (i == tokens.size()) {
                errorTable.errors.emplace_back(tokens[i - 1].stop, 2014, "unexpected EOF");
                errorTable.interrupt("reading tokens");
            }
        }
        while (tokens[i].type == Token::NewLine);
        return tokens[i];
    }

    ClassSymbol *Parser::currentClassSymbol(Symbol* symbol) {
        while (symbol != nullptr && symbol->type != Symbol::Class) {
            symbol = symbol->father;
        }
        return (ClassSymbol *)symbol;
    }
} // Seserot