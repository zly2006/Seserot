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
        for (const auto &item: variables) {
            delete item;
        }
        for (const auto &item: properties) {
            delete item;
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
                // 处理scope
                if (tokens[i].content == "{") {
                    newScope(tokens[i]);
                } else if (tokens[i].content == "}") {
                    endScope(tokens[i]);
                }
            }
            else if (tokens[i].type == Token::Name) {
                token2scope[&tokens[i]] = currentScope;
                if (tokens[i].content == "namespace") {
                    if (i + 1 >= tokens.size() || tokens[i + 1].type != Token::Name) {
                        errorTable.errors.emplace_back(tokens[i].stop, 2011, "namespace not found.");
                        errorTable.interrupt("scanning namespaces");
                    }
                    if (currentFatherSymbol.top()->type != Symbol::Namespace) {
                        errorTable.errors.emplace_back(tokens[i].start, 2501,
                                                       "Namespace can only be defined in namespace scope.");
                        errorTable.interrupt("scanning namespaces");
                    }
                    std::string name = read(i).content;
                    if (currentFatherSymbol.top() != rootNamespace) {
                        name = currentFatherSymbol.top()->name + "." + name;
                    }
                    if (!namespaces.contains(name)) {
                        namespaces.emplace(name, new NamespaceSymbol(currentScope, name));
                    }
                    // 华风夏韵，洛水天依。
                    currentFatherSymbol.push(namespaces[name]);
                }
                else if (tokens[i].content == "class") {
                    if (i + 1 >= tokens.size() || tokens[i + 1].type != Token::Name) {
                        errorTable.errors.emplace_back(tokens[i].stop, 2012, "class name not found.");
                        errorTable.interrupt("scanning classes");
                    }
                    auto mod = parseModifiers(tokens.begin() + i);
                    if (!(mod & Inner)) {
                        mod = static_cast<Modifiers>(mod | Static);
                    }
                    i++;
                    std::string name = tokens[i].content;
                    if (classes.count(name) && !(mod & Partial)) {
                        errorTable.errors.emplace_back(tokens[i].start, 2503, "class definition already exists.");
                        errorTable.interrupt("scanning classes");
                    }
                    ClassSymbol *symbol;
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
                else if (tokens[i].content == "function") {
                    if (i + 1 >= tokens.size() || tokens[i + 1].type != Token::Name) {
                        errorTable.errors.emplace_back(tokens[i].stop, 2013, "method name not found.");
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
                    auto *methodSymbol = new MethodSymbol(nullptr, name, mod,
                                                          {}, {}, nullptr);
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
                    Token &args_optional = read(i);
                    if (args_optional.type == Token::Operator && args_optional.content == "(") {
                        //todo
                        while (read(i).content != ")");
                    }
                    Token &where_optional = read(i);
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
                else if (tokens[i].content == "var" || tokens[i].content == "val") {
                    auto mod = parseModifiers(tokens.begin() + i,
                                              tokens[i].content == "var" ? Mutable : None);
                    auto& t = read(i);
                    auto* classSymbol = currentClassSymbol(currentFatherSymbol.top());
                    auto* methodSymbol = currentMethodSymbol(currentFatherSymbol.top());
                    if (tokens[i].content == "val") {
                        if (mod & Mutable) {
                            //todo: 分配错误码
                            errorTable.errors.emplace_back(t.start, 0, "val cannot be mutable.");
                        }
                    } // val
                    if (methodSymbol != nullptr) {
                        auto symbols = searchSymbol(Symbol::Variable, t.content, currentScope);

                        if (!symbols.empty()) {
                            //todo: 分配错误码
                            std::string msg;
                            msg += "variable ";
                            msg += t.content;
                            msg += " already exists.";
                            errorTable.errors.emplace_back(t.start, 0, msg.c_str());
                        }
                        auto *pItem = new VariableSymbol(currentScope, t.content, classSymbol, mod);
                        variables.push_back(pItem);
                    }
                    else if (classSymbol == currentFatherSymbol.top()) {
                        auto *pItem = new PropertySymbol(currentScope, t.content, classSymbol, mod);
                        properties.push_back(pItem);
                    }
                    else {
                        //todo: 分配错误码
                        errorTable.errors.emplace_back(t.start, 0, "properties should be declared in class scope.");
                    }
                }
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

    void Parser::parseReference() {
        for (auto &item: tokens) {
            if (item.type == Token::Name && item.parsed == Token::Ready) {
                //searchSymbol(static_cast<typename Symbol::Type>(65535), item.content, token2scope[&item]);
                reference[&item] = nullptr;
                // todo
            }
        }
    }

    void Parser::parse() {
        reset();
        scan();
        parseReference();
    }

    /** get the method symbol that contains a specified symbol.
     * @note
     * if input is nullptr, return value will be nullptr as well.
     */
    MethodSymbol *Parser::currentMethodSymbol(Symbol *symbol) {
        while (symbol != nullptr && symbol->type != Symbol::Method) {
            symbol = symbol->father;
        }
        return (MethodSymbol *)symbol;
    }

    std::vector<Symbol *> Parser::searchSymbol(typename Symbol::Type type, const std::string& name, Scope* scope) {
        assert(scope != nullptr);
        assert(!name.empty());
        std::vector<Symbol *> ret;
        if (type & Symbol::Class) {
            for (const auto &item: classes) {
                if (item.first == name && scope->inside(item.second->scope)) {
                    ret.push_back(item.second);
                }
            }
        }
        if (type & Symbol::Variable) {
            for (const auto &item: variables) {
                if (item->name == name && scope->inside(item->scope)) {
                    ret.push_back(item);
                }
            }
        }
        return ret;
    }

    size_t Parser::generateStack(MethodSymbol*methodSymbol) {
        if (!methodSymbol->genericArgs.empty()) {
            return 0;
        }
        else {

        }
        return 0;//todo
    }

    struct Computable {
        enum Type {
            Integer,
            Floating,
            String,
            Symbol,
            Operator,
            ASTNode,
        };
        Type type;
        void* data;
    };

    struct Operator {

    };

    AbstractSyntaxTreeNode* Parser::parseExpression(token_iter& tokenIter) {
        std::vector<Computable> s;
        while (tokenIter != tokens.end()) {
            if (tokenIter->type == Token::Operator) {
                if (!s.empty() && s.back().type != Computable::Operator) {
                    ClassSymbol* type;
                    auto* node = new AbstractSyntaxTreeNode();
                    switch (s.back().type) {
                        case Computable::ASTNode:
                            type = static_cast<AbstractSyntaxTreeNode*>(s.back().data)->typeInferred;

                            break;
                        case Computable::Integer:
                            type = buildIn.longClass;
                            break;
                        case Computable::Floating:
                            type = buildIn.doubleClass;
                            break;
                        case Computable::String:
                            type = buildIn.stringClass;
                            break;
                        case Computable::Symbol:{
                            auto* symbol = static_cast<Symbol*>(s.back().data);
                            switch (symbol->type) {
                                case Symbol::Class:
                                    type = buildIn.classClass;
                                    break;
                                case Symbol::Method:
                                    type = buildIn.functionClass;
                                    break;
                                case Symbol::Property:
                                    type = static_cast<PropertySymbol*>(symbol)->returnType;
                                    break;
                                case Symbol::Variable:
                                    type = static_cast<VariableSymbol*>(symbol)->returnType;
                                    break;
                                case Symbol::Value:
                                case Symbol::Trait:
                                case Symbol::Namespace:
                                    break;
                            }
                            break;
                        }

                        case Computable::Operator:
                            break;
                    }
                    s.pop_back();
                    node->typeInferred = type;
                    node->action = AbstractSyntaxTreeNode::Call;
                    node->children.push_back({});//todo: method
                }
                else {
                    tokenIter++;
                    auto* expr = parseExpression(tokenIter);
                }
                // todo:

            }
        }
    }
} // Seserot
