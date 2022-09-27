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
#include "src/utils/sum_string.h"
#include "src/utils/ByteOrder.h"

#include <utility>
#include <stack>
#include <variant>
#include <cmath>
#include <sstream>
#include <any>
#include <cmath>

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
            currentScope = currentScope->newChildren(thisToken.start,SourcePosition(0,0));
        };
        // 作用：结束当前scope，检查symbol scope并整理currentFatherSymbol
        auto endScope = [&](Token& thisToken) {
            if (currentScope == nullptr) {
                errorTable.errors.emplace_back(thisToken.start, 0, "unexpected \'}\'");//todo
                errorTable.interrupt();
                return;
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
                    auto mod = parseModifiers(tokens.begin() + (long)i);
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
                    Modifiers mod = parseModifiers(tokens.begin() + (long)i);
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
                    auto mod = parseModifiers(tokens.begin() + (long)i,
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

    /**
     * 将一个token前的所有modifier（token形式）解析为Modifier枚举
     *
     * 同时，标记这些token的parse状态为Statement
     * @param it 当前token的迭代器
     * @param def 默认值，可选
     * @return Modifier枚举
     */
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

    /**
     * 读取下一个token并返回读取的结果
     * @param i 当前在第几个token
     * @return 下一个token
     */
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

    /**
     * 当前当最近的ClassSymbol
     * @param symbol 当前symbol，根据这个向上寻找
     * @return symbol为null或没有找到，返回nullptr；否则是ClassSymbol的指针
     */
    ClassSymbol *Parser::currentClassSymbol(Symbol* symbol) {
        while (symbol != nullptr && symbol->type != Symbol::Class) {
            symbol = symbol->father;
        }
        return (ClassSymbol *)symbol;
    }

    /**@todo
     * 全都todo
     */
    void Parser::parseReference() {
        for (auto &item: tokens) {
            if (item.type == Token::Name && item.parsed == Token::Ready) {
                //searchSymbol(static_cast<typename Symbol::Type>(65535), item.content, token2scope[&item]);
                reference[&item] = nullptr;
                // todo
            }
        }
    }

    /**正常的parser流程的啦
     */
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

    AbstractSyntaxTreeNode* Parser::parseExpression(token_iter& tokenIter, char untilBracket) {
        std::vector<std::variant<AbstractSyntaxTreeNode, std::string>> s;
        const std::map<std::string, AbstractSyntaxTreeNode::Actions> actionMap{
                {"+", AbstractSyntaxTreeNode::Add},
                {"-", AbstractSyntaxTreeNode::Subtract},
                {"*", AbstractSyntaxTreeNode::Multiple},
                {"/", AbstractSyntaxTreeNode::Divide},
                {"%", AbstractSyntaxTreeNode::Mod},
                {"&&",AbstractSyntaxTreeNode::LogicAnd},
                {"||",AbstractSyntaxTreeNode::LogicOr},
                {"!",AbstractSyntaxTreeNode::LogicNot},
                {"&",AbstractSyntaxTreeNode::BitAnd},
                {"|",AbstractSyntaxTreeNode::BitOr},
                {"~",AbstractSyntaxTreeNode::BitNot},
                {"<<",AbstractSyntaxTreeNode::LeftShift},
                {">>",AbstractSyntaxTreeNode::RightShift},
                {"^",AbstractSyntaxTreeNode::BitXor}
        };
        const std::vector<std::set<AbstractSyntaxTreeNode::Actions>> priority {
                {
                    AbstractSyntaxTreeNode::Multiple,
                    AbstractSyntaxTreeNode::Divide,
                    AbstractSyntaxTreeNode::Mod,
                 },
                {
                    AbstractSyntaxTreeNode::Add,
                    AbstractSyntaxTreeNode::Subtract,
                },
                {
                    AbstractSyntaxTreeNode::BitAnd,
                    AbstractSyntaxTreeNode::BitOr,
                    AbstractSyntaxTreeNode::BitNot,
                    AbstractSyntaxTreeNode::BitXor,
                    AbstractSyntaxTreeNode::RightShift,
                    AbstractSyntaxTreeNode::LeftShift,
                },
                {
                    AbstractSyntaxTreeNode::LogicNot,
                    AbstractSyntaxTreeNode::LogicAnd,
                    AbstractSyntaxTreeNode::LogicOr,
                },
                {
                    //赋值
                }
        };
        const std::set<AbstractSyntaxTreeNode::Actions> unary = {
                AbstractSyntaxTreeNode::BitNot,
                AbstractSyntaxTreeNode::LogicNot,
        };
        // 1. member access
        // 2. bracket () []
        // 3. infix function & function call
        // 4. * / %
        // 5. + -
        //    bit
        // 6. compare
        // 7. assign
        // 8. comma

        //todo:
        // member access

        // 将tokens解析到s
        // function & bracket
        /* Q: tokenIter何时++？
         * A: 各自独立，直到无法解析
         */
        bool success = false;
        while (tokenIter != tokens.end()) {
            switch (tokenIter->type) {
                case Token::NewLine:
                    if (!untilBracket && s.back().index() == 0) {
                        // 为什么这么写？
                        // 注意此时做不到太精细的分析，同时我们允许用换行分割表达式，那就只能这样了
                        // 对于if，函数之类的，由于括号会继续解析
                        tokenIter++;
                        success = true;
                    }
                    else
                        tokenIter++;
                    break;
                case Token::Operator: {
                    if (tokenIter->content == ";") {
                        if (!untilBracket) {
                            //todo: 分配错误码
                            errorTable.errors.emplace_back(tokenIter->start, 0, "The expression is not ended but "
                                                                                "found a semicolon.");
                            errorTable.interrupt();
                        }
                        success = true;
                        break;
                    }
                    if (tokenIter->content.length() == 1 && tokenIter->content[0] == untilBracket) {
                        success = true;
                        break;//因为括号结束
                    }

                    if (!s.empty() && s.back().index() == 0 &&
                        tokenIter->type == Token::Operator && tokenIter->content == "(") {
                        // function
                        // this field's purpose is to identify which function should be called.
                        ClassSymbol *type = nullptr;
                        auto *node = new AbstractSyntaxTreeNode();
                        s.pop_back();
                        node->typeInferred = type;
                        node->action = AbstractSyntaxTreeNode::Call;
                        node->children.push_back({});//todo: method
                        tokenIter++;
                        return node;
                    }
                    else if (tokenIter->type == Token::Operator && tokenIter->content == "(") {
                        // brackets
                        tokenIter++;
                        auto *expr = parseExpression(tokenIter, ')');
                        if (expr == nullptr) {
                            // todo: 分配错误码
                            errorTable.errors.emplace_back(tokenIter->start, 0, "invalid bracket.");
                            errorTable.interrupt();
                        }
                        if (tokenIter->type == Token::Operator && tokenIter->content == ")") {
                            s.emplace_back(*expr);
                            tokenIter++;
                        }
                        else {
                            // todo: 分配错误码
                            errorTable.errors.emplace_back(tokenIter->start, 0, "invalid bracket.");
                            errorTable.interrupt();
                        }
                    }
                    else {
                        s.emplace_back(tokenIter->content);
                        tokenIter++;
                    }
                    // todo:
                    break;
                }
                case Token::Name: {
                    //infix function
                    if (reference.contains(tokenIter.base())) {
                        auto symbol = reference[tokenIter.base()];
                        if (symbol->type == Symbol::Method) {
                            auto function = (MethodSymbol*)symbol;
                            if (function->modifiers & Infix) {
                                // infix call
                            }
                        }
                    }
                    break;
                }
                case Token::Number: {
                    AbstractSyntaxTreeNode::Actions action = AbstractSyntaxTreeNode::LiteralLong;
                    bool negative = false;
                    // negative number
                    if (!s.empty() && s.back().index() == 1) {
                        std::string op = std::get<std::string>(s.back());
                        if (op == "-") {
                            if (s.size() == 1 || // 只有一个元素，是负号
                                (s.size() >= 2 && (s.end() - 2)->index() == 1)//前一个是符号，是负号
                                    ) {
                                s.pop_back();
                                negative = true;
                            }
                        }
                    }
                    // parse number
                    std::string toParse = tokenIter->content;
                    long power = 0;//e后面的指数
                    size_t size;
                    AbstractSyntaxTreeNode node;
                    std::variant<long long, long double, unsigned long long> v;
                    std::string suffix;
                    if (auto index = toParse.find('e'); index != std::string::npos) {
                        std::string str = toParse.substr(index + 1);
                        char *end;
                        power = strtol(str.c_str(), &end, 10);
                        suffix = end;
                        toParse = toParse.substr(0, index);
                        action = AbstractSyntaxTreeNode::LiteralDouble;
                    }
                    // 'e' or '.' is double
                    if (tokenIter->content.find('.') != -1 || action == AbstractSyntaxTreeNode::LiteralDouble) {
                        // 小数
                        action = AbstractSyntaxTreeNode::LiteralDouble;
                        char *end;
                        double value = strtod(tokenIter->content.c_str(), &end);
                        if (!suffix.empty() || strlen(end) != 0) {
                            // e.g.
                            // 1.3ue5u
                            //todo: 分配错误码
                            errorTable.errors.emplace_back(tokenIter->start, 0, "double literal cannot have suffix.");
                        }
                        if (negative) {
                            value = -value;
                        }
                        value *= pow(10, power);
                        if (isnan(value) || isinf(value)) {
                            errorTable.errors.emplace_back(tokenIter->start, 0, "bad literal: NaN / Inf");
                        }
                        node.data = new char[8];
                        node.dataLength = 8;
                        memcpy(node.data, &value, 8);
                    }
                    else {
                        char* end = new char[8];
                        size = string2FitNumber(toParse, end, negative);
                        if (size == 256) {
                            //todo: 分配错误码
                            errorTable.errors.emplace_back(tokenIter->start, 0, "number size overflowed.");
                        }
                        if (size == 0) {
                            errorTable.errors.emplace_back(tokenIter->start, 1000,
                                                          "parse number literal @"+ HERE);
                        }
                        node.dataLength = size;
                        node.data = end;
                    }

                    node.action = action;

                    //check suffix(parsed above here)
                    for (int i = 0; i < suffix.length(); ++i) {
                        if (suffix.find(suffix[i], i + 1) != std::string::npos) {
                            // todo:errcode
                            errorTable.errors.emplace_back(tokenIter->start, 0, "repeated number modifier.");
                            break;
                        }
                    }

                    s.emplace_back(node);
                    tokenIter++;
                    break;
                }
                case Token::Literal: {
                    AbstractSyntaxTreeNode node;
                    node.action = AbstractSyntaxTreeNode::LiteralString;
                    node.dataLength = tokenIter->content.length();
                    node.data = new char[node.dataLength];
                    memcpy(node.data, tokenIter->content.c_str(), node.dataLength);
                    s.emplace_back(node);
                    tokenIter++;
                    break;
                }
                default:
                    break;
            }
            if (success) break;
        }

        if (!success) {
            // todo:分配错误码
            errorTable.errors.emplace_back(tokenIter->start, 0, "unexpected EOF while parsing expressions.");
            errorTable.interrupt();
        }

        for (int i = 0; i < priority.size(); ++i) {
            auto &set = priority.at(i);
            for (auto it = s.begin(); it != s.end(); it++) {
                if (it->index() == 1) {
                    AbstractSyntaxTreeNode::Actions action = actionMap.at(std::get<std::string>(*it));
                    if (set.contains(action)) {
                        if (unary.contains(action)) {
                            AbstractSyntaxTreeNode node;
                            node.action = action;
                            // it -> op
                            it = s.erase(it);
                            // it -> obj
                            node.children.push_back(std::get<AbstractSyntaxTreeNode>(*it));
                            it = s.erase(it);
                            // it -> obj+1
                            it = s.insert(it, node);
                            // it -> new obj
                        }
                    }
                    else if (it == s.begin() || it + 1 == s.end()) {//双目运算符但是ASTNode不够
                        //todo: 分配错误码
                        errorTable.interrupt();
                    }
                    else {
                        AbstractSyntaxTreeNode node;
                        node.action = action;
                        // it -> op
                        it--;
                        // it -> op-1
                        node.children.push_back(std::get<AbstractSyntaxTreeNode>(*it));
                        it = s.erase(it);
                        // it -> op
                        it = s.erase(it);
                        // it -> op+1
                        node.children.push_back(std::get<AbstractSyntaxTreeNode>(*it));
                        it = s.erase(it);
                        // it -> op+2
                        it = s.insert(it, node);
                    }
                }
            }
        }

        if (s.size() == 1 && s.front().index() == 0) {
            return new AbstractSyntaxTreeNode(std::get<0>(s.front()));
        }
        return nullptr;
    }

    template<class T>
    std::optional<T> Parser::convertToNumber(const std::string& str) {
        T t;
        std::stringstream ss;
        ss << str;

        return std::optional<T>();
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"
    /**
     * 将包含正数的字符串转为数字并检查溢出
     * @param str 正数，不含非数字字符
     * @param ptr 返回一个持久化保存的对象，类型是char数组，可以delete，保证是小端序
     * @param negative 表示这个数字是不是false
     * @return 数据的长度，单位字节，最大为8，如果溢出，则返回256，如果不符合格式，返回0
     */
    size_t Parser::string2FitNumber(const std::string &str, char *ptr, bool negative) {
        std::stringstream ss;
        unsigned long long ll;
        ss << str;
        ss >> ll;
        if (negative) {
            ll = -ll;
        }
        if (!ss.eof() || (ll & 0x8000000000000000ull)) {
            return 0;
        }
        if (!ss) {
            return 256;
        }
        if (ll & 0xffffffff00000000ull) {
            static_assert(sizeof(long) == 8);
            memcpy(ptr, &ll, 8);
            return 8;
        }
        else if (ll & 0xffff0000ull) {
            static_assert(sizeof(int) == 4);
            if constexpr (getEndianOrder() == hl_endianness::HL_BIG_ENDIAN)
                std::reverse_copy(&ll + 4, &ll + 8, ptr);
            else
                memcpy(ptr, &ll, 4);
            return 4;
        }
        else if (ll & 0xff00ull) {
            static_assert(sizeof(short) == 2);
            if constexpr (getEndianOrder() == hl_endianness::HL_BIG_ENDIAN)
                std::reverse_copy(&ll + 6, &ll + 8, ptr);
            else
                memcpy(ptr, &ll, 2);
            return 2;
        }
        else {
            static_assert(sizeof(char) == 1);
            if constexpr (getEndianOrder() == hl_endianness::HL_BIG_ENDIAN)
                std::reverse_copy(&ll + 7, &ll + 8, ptr);
            else
                memcpy(ptr, &ll, 1);
            return 1;
        }
    }
#pragma clang diagnostic pop

    Token &Parser::expectIdentifier(size_t &pos) {
        pos++;
        if (pos == tokens.size()) {
            //todo: 分配错误码
            errorTable.errors.emplace_back(tokens.back().stop, 0, "unexpected EOF");
            errorTable.interrupt();
        }
        if (tokens[pos].type != Token::Name) {
            //todo: 分配错误码
            errorTable.errors.emplace_back(tokens.back().stop, 0, "Expected an identifier.");
            errorTable.interrupt();
        }
        return tokens[pos];
    }

    Token &Parser::expectOperator(size_t &pos) {
        pos++;
        if (pos == tokens.size()) {
            //todo: 分配错误码
            errorTable.errors.emplace_back(tokens.back().stop, 0, "unexpected EOF");
            errorTable.interrupt();
        }
        if (tokens[pos].type != Token::Operator) {
            //todo: 分配错误码
            errorTable.errors.emplace_back(tokens.back().stop, 0, "Expected an operator.");
            errorTable.interrupt();
        }
        return tokens[pos];
    }

    bool Parser::expectIdentifier(size_t &pos, const std::string& content) {
        pos++;
        if (pos == tokens.size()) {
            //todo: 分配错误码
            errorTable.errors.emplace_back(tokens.back().stop, 0, "unexpected EOF");
            errorTable.interrupt();
        }
        return tokens[pos].type == Token::Operator && tokens[pos].content == content;
    }
} // Seserot
