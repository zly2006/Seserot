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

#include <any>
#include <cmath>
#include <sstream>
#include <stack>
#include <utility>
#include <variant>

#include "Symbol.h"
#include "SymbolTable.h"
#include "ast/ASTNode.h"
#include "ast/AccessVariableNode.h"
#include "ast/BinaryOperatorNode.h"
#include "ast/BlockNode.h"
#include "ast/FloatingConstantNode.h"
#include "ast/FunctionInvokeNode.h"
#include "ast/IfElseNode.h"
#include "ast/IntegerConstantNode.h"
#include "ast/ReturnNode.h"
#include "ast/StringConstantNode.h"
#include "ast/UnaryOperatorNode.h"
#include "utils/ByteOrder.h"
#include "utils/common.h"
#include "utils/sum_string.h"

namespace Seserot {
    void Parser::scan() {
        Scope *currentScope = &root;
        auto *rootNamespace = new NamespaceSymbol(currentScope, "<root>");
        symbolTable.emplace(std::unique_ptr<NamespaceSymbol>(rootNamespace));
        std::stack<SymbolWithChildren *> currentFatherSymbol;
        currentFatherSymbol.emplace(rootNamespace);
        // init finish

        auto newScope = [&](Token &thisToken) {
            currentScope = currentScope->newChildren(thisToken.start, SourcePosition(0, 0));
        };
        // 作用：结束当前scope，检查symbol scope并整理currentFatherSymbol
        auto endScope = [&](Token &thisToken) {
            if (currentScope == nullptr) {
                // todo: 分配错误码
                appendError(0, "Unexpected \'}\'", thisToken.start);
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
                } else if (tokens[i].content == "->") {
                    // lambda
                    // todo
                }
            } else if (tokens[i].type == Token::Name) {
                token2scope[&tokens[i]] = currentScope;
                if (tokens[i].content == "namespace") {
                    if (i + 1 >= tokens.size() || tokens[i + 1].type != Token::Name) {
                        appendError(2011, "namespace name expected", tokens[i].start);
                        errorTable.interrupt("scanning namespaces");
                    }
                    if (currentFatherSymbol.top()->type != Symbol::Namespace) {
                        appendError(2012, "namespace can only be defined in namespace", tokens[i].start);
                        errorTable.interrupt("scanning namespaces");
                    }
                    std::string name = read(i).content;
                    if (currentFatherSymbol.top() != rootNamespace) {
                        name = currentFatherSymbol.top()->name + "." + name;
                    }
                    auto *ns = new NamespaceSymbol(currentScope, name);
                    ns->signature = name;
                    symbolTable.emplace(std::unique_ptr<NamespaceSymbol>(ns));
                    if (tokens[i].content == "{" && tokens[i].type == Token::Operator) {
                        newScope(tokens[i]);
                        ns->childScope = currentScope;
                        break;
                    }  // 华风夏韵，洛水天依。
                    else {
                        ns->childScope = &root;
                    }
                    currentFatherSymbol.push(ns);
                } else if (tokens[i].content == "class") {
                    if (i + 1 >= tokens.size() || tokens[i + 1].type != Token::Name) {
                        errorTable.errors.emplace_back(
                                std::make_unique<CompilerError>(tokens[i].stop, 2012, "class name not found."));
                        errorTable.interrupt("scanning classes");
                    }
                    auto mod = parseModifiers(tokens.begin() + (long)i);
                    if (!(mod & Inner)) {
                        mod = static_cast<Modifiers>(mod | Static);
                    }
                    i++;
                    std::string name = tokens[i].content;
                    auto *symbol = new ClassSymbol(currentScope, name, {}, nullptr, mod, {});
                    if (auto *pf = currentNamespaceSymbol(currentFatherSymbol.top()); pf != nullptr) {
                        symbol->signature = pf->signature + "::" + name;
                    } else {
                        symbol->signature = "::" + name;
                    }
                    if (!symbolTable.emplace(std::unique_ptr<ClassSymbol>(symbol))) {
                        errorTable.errors.emplace_back(std::make_unique<CompilerError>(
                                tokens[i].start, 2503, "class definition already exists."));
                        errorTable.interrupt("scanning classes");
                    }
                    while (true) {
                        i++;  // todo
                        if (tokens[i].content == "{" && tokens[i].type == Token::Operator) {
                            newScope(tokens[i]);
                            symbol->childScope = currentScope;
                            currentFatherSymbol.push(symbol);
                            break;
                        }
                    }
                } else if (tokens[i].content == "function") {
                    if (i + 1 >= tokens.size() || tokens[i + 1].type != Token::Name) {
                        errorTable.errors.emplace_back(
                                std::make_unique<CompilerError>(tokens[i].stop, 2013, "method name not found."));
                        errorTable.interrupt("scanning methods");
                    }
                    Modifiers mod = parseModifiers(tokens.begin() + (long)i);
                    i++;
                    std::string name = tokens[i].content;
                    if (currentFatherSymbol.top() != rootNamespace) {
                        currentFatherSymbol.top()->name + "." + name;
                    }
                    auto *methodSymbol = new MethodSymbol(nullptr, name, mod, {}, {}, nullptr);
                    tokens[i].parsed = Token::Statement;
                    ClassSymbol *father = currentClassSymbol(currentFatherSymbol.top());
                    methodSymbol->father = father;
                    if (father != nullptr && father->modifiers & Static) {
                        if (!(methodSymbol->modifiers & Static)) {
                            errorTable.errors.emplace_back(
                                    std::make_unique<CompilerError>(tokens[i].start, 0, ""));  // todo
                        }
                    }
                    if (i != tokens.size() - 1) {
                        if (tokens[i].type == Token::Operator && tokens[i].content == "<") {
                            Token &genericName = read(i);
                            if (genericName.type != Token::Name) {
                                // todo: error code
                                appendError(0, "generic arg name expected", genericName.start);
                                errorTable.interrupt();
                            }
                            methodSymbol->genericArgs.push_back(
                                    ClassSymbol(nullptr, genericName.content, {}, nullptr, Modifiers::None, {}));
                        }
                    }
                    Token &args_optional = read(i);
                    if (args_optional.type == Token::Operator && args_optional.content == "(") {
                        // todo
                        while (read(i).content != ")")
                            ;
                    }
                    Token &where_optional = read(i);
                    if (where_optional.type == Token::Name && where_optional.content == "where") {
                        // todo
                    } else if (where_optional.type == Token::Operator && where_optional.content == "{") {
                        newScope(tokens[i]);
                        methodSymbol->childScope = currentScope;
                        currentFatherSymbol.push(methodSymbol);
                        for (auto &item: methodSymbol->genericArgs) {
                            item.father = methodSymbol;
                            item.scope = currentScope;
                        }
                    }

                    if (!symbolTable.emplace(std::unique_ptr<MethodSymbol>(methodSymbol))) {
                        appendError(2504, "method definition already exists", tokens[i].start);
                        errorTable.interrupt("scanning methods");
                    }
                } else if (tokens[i].content == "var" || tokens[i].content == "val") {
                    auto mod = parseModifiers(tokens.begin() + (long)i, tokens[i].content == "var" ? Mutable : None);
                    auto &t = read(i);
                    auto *classSymbol = currentClassSymbol(currentFatherSymbol.top());
                    auto *methodSymbol = currentMethodSymbol(currentFatherSymbol.top());
                    if (tokens[i].content == "val") {
                        if (mod & Mutable) {
                            // todo: 分配错误码
                            appendError(0, "val cannot be mutable", tokens[i].start);
                        }
                    }  // val
                    if (methodSymbol != nullptr) {
                        auto *pItem = new VariableSymbol(currentScope, t.content, classSymbol, mod, nullptr);
                        if (!symbolTable.emplace(std::unique_ptr<VariableSymbol>(pItem))) {
                            // todo: 分配错误码
                            std::string msg;
                            msg += "variable ";
                            msg += t.content;
                            msg += " already exists.";
                            errorTable.errors.emplace_back(std::make_unique<CompilerError>(t.start, 0, msg.c_str()));
                        }
                    } else if (classSymbol == currentFatherSymbol.top()) {
                        auto *pItem = new PropertySymbol(currentScope, t.content, classSymbol, mod);
                        if (!symbolTable.emplace(std::unique_ptr<PropertySymbol>(pItem))) {}
                    } else {
                        // todo: 分配错误码
                        appendError(0, "variable definition must be in class or method", t.start);
                    }
                } else if (tokens[i].content == "import") {
                } else if (tokens[i].content == "return") {
                    if (currentMethodSymbol(currentFatherSymbol.top()) == nullptr) {
                        appendError(0, "return statement must be in method", tokens[i].start);
                    } else {
                        tokens[i].parsed = Token::Statement;
                        if (i + 1 != tokens.size() && tokens[i + 1].content == "@" &&
                            tokens[i + 1].type == Token::Operator) {
                            i++;
                            auto &t = read(i);
                            if (t.type != Token::Name) {
                                appendError(0, "name expected", t.start);
                            } else {
                                tokens[i].parsed = Token::Statement;
                                reference.emplace(&tokens[i], currentMethodSymbol(currentFatherSymbol.top()));
                            }
                        }
                    }
                }
            }
        }
        if (currentScope != &root) {
            appendError(0, "scope not closed", tokens.back().stop);
        }
        if (!errorTable.errors.empty()) {
            errorTable.interrupt("scanning failed.");
        }
    }

    Parser::Parser(std::vector<Token> tokens, ErrorTable &errorTable):
            tokens(std::move(tokens)), errorTable(errorTable) {}

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
                    appendError(0, "modifier duplicated", first->start);
                }
                ret.insert(first->content);
            } else if (first->type != Token::NewLine) {
                // other
                break;
            }
            first->parsed = Token::Statement;
            if (first == tokens.begin()) break;
            first--;
        }
        unsigned int m = def;
        bool accessibilityParsed = false;
        bool mutableParsed = false;
        for (auto &mo: ret) {
            if (mo == "public" || mo == "private" || mo == "internal" || mo == "protected") {
                if (accessibilityParsed) {
                    appendError(2505, "conflict accessibility modifier.", it->start);
                    errorTable.interrupt();
                }
                accessibilityParsed = true;
                if (mo == "public") {
                } else if (mo == "private") {
                    m |= PrivateInternal;
                    m |= PrivateProtected;
                } else if (mo == "protected") {
                    m |= PrivateProtected;
                } else if (mo == "internal") {
                    m |= PrivateInternal;
                }
            } else if (mo == "mutable" || mo == "immutable") {
                if (mutableParsed) {
                    appendError(2505, "conflict mutable modifier.", it->start);
                }
                mutableParsed = true;
                if (mo == "mutable") {
                    m |= Mutable;
                }
            } else if (mo == "static") {
                m |= Static;
            } else if (mo == "final") {
                m |= Final;
            } else if (mo == "abstract") {
                m |= Abstract;
            } else if (mo == "async") {
                m |= Async;
            } else if (mo == "readonly") {
                m |= Readonly;
            } else if (mo == "operator") {
                m |= Operator;
            } else if (mo == "value") {
                m |= ValueType;
            } else if (mo == "inner") {
                m |= Inner;
            }
        }
        if ((m & Final) && (m & Static)) {
            appendError(2505, "final and static cannot be used together.", it->start);
        }
        return (Modifiers)m;
    }

    /**
     * 读取下一个token并返回读取的结果
     * @param i 当前在第几个token
     * @return 下一个token
     */
    Token &Parser::read(size_t &i) {
        do {
            i++;
            if (i == tokens.size()) {
                appendError(2014, "unexpected end of file.", tokens.back().stop);
                errorTable.interrupt("reading tokens");
            }
        } while (tokens[i].type == Token::NewLine);
        return tokens[i];
    }

    /**
     * 当前当最近的ClassSymbol
     * @param symbol 当前symbol，根据这个向上寻找
     * @return symbol为null或没有找到，返回nullptr；否则是ClassSymbol的指针
     */
    ClassSymbol *Parser::currentClassSymbol(Symbol *symbol) {
        while (symbol != nullptr && symbol->type != Symbol::Class) {
            symbol = symbol->father;
        }
        return (ClassSymbol *)symbol;
    }

    /**@todo 全都todo
     */
    void Parser::parseReference() {
        for (auto &item: tokens) {
            if (item.type == Token::Name && item.parsed == Token::Ready) {
                // searchSymbol(static_cast<typename Symbol::Type>(65535),
                // item.content, token2scope[&item]);

                reference.find(&item);
                // todo
            }
        }
    }

    /**
     * 正常的parser流程的啦
     */
    void Parser::parse() {
        scan();
        std::vector<std::string_view> imports;
        for (auto it = tokens.begin(); it != tokens.end(); ++it) {
            if (it->content == "import" && it->type == Token::Name) {
                if (it->parsed != Token::Ready || it + 1 == tokens.end()) {
                    // todo:分配错误码
                    appendError(0, "import statement error", it->start);
                }
                it->parsed = Token::Statement;
                it++;
                if (it->type != Token::Name) {
                    // todo:分配错误码
                    appendError(0, "import statement error", it->start);
                }
                imports.push_back(it->content);
            }
        }
        importSymbols(imports);
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

    [[deprecated("llvm can solve this problem")]] size_t Parser::generateStack(MethodSymbol *methodSymbol) {
        if (!methodSymbol->genericArgs.empty()) {
            return 0;
        } else {
        }
        return 0;  // todo
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

    /**
     * 解析表达式
     * @param tokenIter 表达式后一个token
     * @param untilBracket 遇到这个括号停止eat，进入后续的解析
     * @return
     */
    std::unique_ptr<AST::ASTNode> Parser::parseExpression(token_iter &tokenIter, char untilBracket) {
        AST::ASTNode::Actions actions;
        std::vector<std::variant<std::unique_ptr<AST::ASTNode>, std::string>> s;
        const std::map<std::string, AST::ASTNode::Actions> actionMap{
                {"+", AST::ASTNode::Actions::Add},         {"-", AST::ASTNode::Actions::Subtract},
                {"*", AST::ASTNode::Actions::Multiple},    {"/", AST::ASTNode::Actions::Divide},
                {"%", AST::ASTNode::Actions::Mod},         {"&&", AST::ASTNode::Actions::LogicAnd},
                {"||", AST::ASTNode::Actions::LogicOr},    {"!", AST::ASTNode::Actions::LogicNot},
                {"&", AST::ASTNode::Actions::BitAnd},      {"|", AST::ASTNode::Actions::BitOr},
                {"~", AST::ASTNode::Actions::BitNot},      {"<<", AST::ASTNode::Actions::LeftShift},
                {">>", AST::ASTNode::Actions::RightShift}, {"^", AST::ASTNode::Actions::BitXor}};
        const std::vector<std::set<AST::ASTNode::Actions>> priority{{
                                                                            AST::ASTNode::Actions::Multiple,
                                                                            AST::ASTNode::Actions::Divide,
                                                                            AST::ASTNode::Actions::Mod,
                                                                    },
                                                                    {
                                                                            AST::ASTNode::Actions::Add,
                                                                            AST::ASTNode::Actions::Subtract,
                                                                    },
                                                                    {
                                                                            AST::ASTNode::Actions::BitAnd,
                                                                            AST::ASTNode::Actions::BitOr,
                                                                            AST::ASTNode::Actions::BitNot,
                                                                            AST::ASTNode::Actions::BitXor,
                                                                            AST::ASTNode::Actions::RightShift,
                                                                            AST::ASTNode::Actions::LeftShift,
                                                                    },
                                                                    {
                                                                            AST::ASTNode::Actions::LogicNot,
                                                                            AST::ASTNode::Actions::LogicAnd,
                                                                            AST::ASTNode::Actions::LogicOr,
                                                                    },
                                                                    {
                                                                            //赋值
                                                                    }};
        const std::set<AST::ASTNode::Actions> unary = {
                AST::ASTNode::Actions::BitNot,
                AST::ASTNode::Actions::LogicNot,
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

        // todo:
        //  member access

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
                        // 为什么用最后一个的index判断结束？
                        // 注意此时做不到太精细的分析，同时我们允许用换行分割表达式，那就只能这样了
                        // 对于if，函数之类的，由于括号会继续解析
                        tokenIter++;
                        success = true;
                    } else
                        tokenIter++;
                    break;
                case Token::Operator: {
                    if (tokenIter->content == ";") {
                        if (untilBracket) {
                            // todo: 分配错误码
                            appendError(0, "unexpected ';' in expression.", tokenIter->start);
                            return nullptr;
                        }
                        success = true;
                        break;
                    }
                    if (tokenIter->content.length() == 1 && tokenIter->content[0] == untilBracket) {
                        success = true;
                        tokenIter++;
                        break;  //因为括号结束
                    }

                    else if (tokenIter->content == "(") {
                        // brackets
                        tokenIter++;
                        auto expr = parseExpression(tokenIter, ')');
                        if (!expr) {
                            // todo: 分配错误码
                            appendError(0, "unexpected ')'", tokenIter->start);
                            errorTable.interrupt();
                        }
                        s.emplace_back(std::move(expr));
                    } else {
                        if (!actionMap.contains(tokenIter->content)) {
                            success = true;
                            break;  // 因为不是运算符结束，不需要前进到
                        }
                        s.emplace_back(tokenIter->content);
                        tokenIter++;
                    }
                    break;
                }
                case Token::Name: {
                    if (tokenIter->content == "true" || tokenIter->content == "false") {
                        s.emplace_back(std::make_unique<AST::IntegerConstantNode>(tokenIter->content == "true"));
                        tokenIter++;
                        break;
                    }
                    auto symbols = symbolTable.lookup(tokenIter->content, token2scope[&*tokenIter]);
                    if (tokenIter + 1 != tokens.end() && (tokenIter + 1)->type == Token::Operator &&
                        (tokenIter + 1)->content == "(") {
                        // function
                        // this field's purpose is to identify which function
                        // should be called. todo: 这里已经要进行重载决策了
                        // 同时，之前的类型推断
                        MethodSymbol *methodSymbol = nullptr;
                        // todo: 重载决策
                        symbolTable.lookup(tokenIter->content, token2scope[&*tokenIter]);
                        if (!methodSymbol) {
                            // todo: 分配错误码
                            appendError(0, "undefined function '" + tokenIter->content + "'.", tokenIter->start);
                        }
                        std::unique_ptr<AST::ASTNode> node = std::make_unique<AST::FunctionInvokeNode>(methodSymbol);
                        s.pop_back();
                        // todo: set args
                        tokenIter++;
                        s.emplace_back(std::move(node));
                    }
                    // name + name: infix or disallowed
                    if (!s.empty() && s.back().index() == 0) {
                        // infix function
                        bool found = false;
                        for (const auto &item: symbols) {
                            if (item->type == Symbol::Type::Method) {
                                auto methodSymbol = dynamic_cast<MethodSymbol *>(item);
                                if (methodSymbol->modifiers & Infix) {
                                    auto node = std::make_unique<AST::FunctionInvokeNode>(methodSymbol);
                                    node->args.push_back(std::move(std::get<0>(s.back())));
                                    s.pop_back();
                                    s.emplace_back(std::move(node));
                                    tokenIter++;
                                    found = true;
                                    break;
                                }
                            }
                        }
                        if (found) {
                            break;
                        }
                        // 这个时候，如果是name+name，那就是错误的
                        // 所以，中止表达式解析防止错误发生
                        success = true;
                        break;
                    }
                    // variable access
                    if (!symbols.empty()) {
                        bool found = false;
                        for (const auto &item: symbols) {
                            if (item->type == Symbol::Type::Variable) {
                                auto node =
                                        std::make_unique<AST::AccessVariableNode>(dynamic_cast<VariableSymbol *>(item));
                                s.emplace_back(std::move(node));
                                found = true;
                                tokenIter++;
                                break;
                            }
                        }
                        if (found) break;
                    }
                    break;
                }
                case Token::Number: {
                    AST::ASTNode::Actions action = AST::ASTNode::Actions::LiteralLong;
                    bool negative = false;
                    // negative number
                    if (!s.empty() && s.back().index() == 1) {
                        std::string op = std::get<std::string>(s.back());
                        if (op == "-") {
                            if (s.size() == 1 ||                                // 只有一个元素，是负号
                                (s.size() >= 2 && (s.end() - 2)->index() == 1)  //前一个是符号，是负号
                            ) {
                                s.pop_back();
                                negative = true;
                            }
                        }
                    }
                    // parse number
                    std::string toParse = tokenIter->content;
                    long power = 0;  // e后面的指数
                    size_t size;
                    std::unique_ptr<AST::ASTNode> node;
                    std::variant<long long, long double, unsigned long long> v;
                    std::string suffix;
                    if (auto index = toParse.find('e'); index != std::string::npos) {
                        std::string str = toParse.substr(index + 1);
                        char *end;
                        power = strtol(str.c_str(), &end, 10);
                        suffix = end;
                        toParse = toParse.substr(0, index);
                        action = AST::ASTNode::Actions::LiteralDouble;
                    }
                    // 'e' or '.' is double
                    if (tokenIter->content.find('.') != -1 || action == AST::ASTNode::Actions::LiteralDouble) {
                        // 小数
                        action = AST::ASTNode::Actions::LiteralDouble;
                        char *end;
                        double value = strtod(tokenIter->content.c_str(), &end);
                        if (!suffix.empty() || strlen(end) != 0) {
                            // e.g.
                            // 1.3ue5u
                            // todo: 分配错误码
                            appendError(0, "invalid number suffix.", tokenIter->start);
                        }
                        if (negative) {
                            value = -value;
                        }
                        value *= pow(10, power);
                        if (std::isnan(value) || std::isinf(value)) {
                            appendError(0, "Double literal cannot be NaN / Inf.", tokenIter->start);
                        }
                        node = std::make_unique<AST::FloatingConstantNode>(value);
                    } else {
                        size_t val;

                        node = string2FitNumber(toParse, val, negative);
                        if (!node) {
                            // todo: 分配错误码
                            appendError(0, "number size overflow.", tokenIter->start);
                            node = std::make_unique<AST::IntegerConstantNode>(0);
                        }
                    }

                    // check suffix(parsed above here)
                    for (int i = 0; i < suffix.length(); ++i) {
                        if (suffix.find(suffix[i], i + 1) != std::string::npos) {
                            // todo: 分配错误码
                            appendError(0, "repeated number modifier.", tokenIter->start);
                            break;
                        }
                    }

                    s.emplace_back(std::move(node));
                    tokenIter++;
                    break;
                }
                case Token::Literal: {
                    s.emplace_back(std::make_unique<AST::StringConstantNode>(tokenIter->content));
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
            appendError(0, "unexpected EOF while parsing expressions.", tokenIter->start);
            errorTable.interrupt();
        }

        for (const auto &set: priority) {
            for (auto it = s.begin(); it != s.end(); it++) {
                if (it->index() == 1) {
                    AST::ASTNode::Actions action = actionMap.at(std::get<std::string>(*it));
                    if (set.contains(action)) {
                        if (unary.contains(action)) {
                            auto node = std::make_unique<AST::UnaryOperatorNode>();
                            node->action = action;
                            // it -> op
                            it = s.erase(it);
                            // it -> obj
                            node->child = std::move(std::get<0>(*it));
                            it = s.erase(it);
                            // it -> obj+1
                            it = s.insert(it, std::move(node));
                            // it -> new obj
                        }
                    } else if (it == s.begin() || it + 1 == s.end()) {  //双目运算符但是ASTNode不够
                        // todo: 分配错误码
                        errorTable.interrupt();
                    } else {
                        auto node = std::make_unique<AST::BinaryOperatorNode>();
                        node->action = action;
                        // it -> op
                        it--;
                        // it -> op-1
                        node->left = std::move(std::get<0>(*it));
                        it = s.erase(it);
                        // it -> op
                        it = s.erase(it);
                        // it -> op+1
                        node->right = std::move(std::get<0>(*it));
                        it = s.erase(it);
                        // it -> op+2
                        it = s.insert(it, std::move(node));
                    }
                }
            }
        }

        if (s.size() == 1 && s.front().index() == 0) {
            return std::move(std::get<0>(s.front()));
        }
        return nullptr;
    }

#pragma clang diagnostic pop

    template <class T>
    std::optional<T> Parser::convertToNumber(const std::string &str) {
        T t;
        std::stringstream ss;
        ss << str;

        return std::optional<T>();
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"
#pragma ide diagnostic ignored "UnreachableCode"

    /**
     * 将包含正数的字符串转为数字并检查溢出
     * @param str 正数，不含非数字字符
     * @param ret 返回值
     * @param negative 表示这个数字是不是false
     * @return
     * 数据的长度，单位字节，最大为8，如果溢出，则返回256，如果不符合格式，返回0
     */
    std::unique_ptr<AST::IntegerConstantNode> Parser::string2FitNumber(const std::string &str, size_t &ret,
                                                                       bool negative) {
        std::stringstream ss;
        unsigned long long ll;
        ss << str;
        ss >> ll;
        auto processNegative = [&]() {
            if (negative) ll = -ll;
            ret = ll;
        };
        if (!ss.eof() || (ll & 0x8000000000000000ull)) {
            return nullptr;
        }
        if (!ss) {
            return nullptr;
        }
        if (ll & 0xffffffff00000000ull) {
            return std::make_unique<AST::IntegerConstantNode>((uint64)ll);
        } else if (negative && (ll & 80000000ull)) {
            return std::make_unique<AST::IntegerConstantNode>((int64)-ll);
        } else if (ll & 0xffff0000ull) {
            return std::make_unique<AST::IntegerConstantNode>((uint32)ll);
        } else if (negative && (ll & 8000ull)) {
            return std::make_unique<AST::IntegerConstantNode>((int32)-ll);
        } else if (ll & 0xff00ull) {
            return std::make_unique<AST::IntegerConstantNode>((uint16)ll);
        } else if (negative && (ll & 80ull)) {
            return std::make_unique<AST::IntegerConstantNode>((int16)-ll);
        } else {
            return std::make_unique<AST::IntegerConstantNode>((int8)ll);
        }
    }

#pragma clang diagnostic pop

    Token &Parser::expectIdentifier(size_t &pos) {
        pos++;
        if (pos == tokens.size()) {
            // todo: 分配错误码
            appendError(0, "got EOF but expected an identifier.", tokens[pos - 1].start);
            errorTable.interrupt();
        }
        if (tokens[pos].type != Token::Name) {
            // todo: 分配错误码
            appendError(0, "expected identifier.", tokens[pos].start);
            errorTable.interrupt();
        }
        return tokens[pos];
    }

    Token &Parser::expectOperator(size_t &pos) {
        pos++;
        if (pos == tokens.size()) {
            // todo: 分配错误码
            appendError(0, "unexpected EOF", tokens.back().stop);
            errorTable.interrupt();
        }
        if (tokens[pos].type != Token::Operator) {
            // todo: 分配错误码
            appendError(0, "expected operator.", tokens[pos].start);
            errorTable.interrupt();
        }
        return tokens[pos];
    }

    bool Parser::expectIdentifier(size_t &pos, const std::string &content) {
        pos++;
        if (pos == tokens.size()) {
            // todo: 分配错误码
            appendError(0, "got EOF but expected an identifier.", tokens[pos - 1].start);
            errorTable.interrupt();
        }
        return tokens[pos].type == Token::Operator && tokens[pos].content == content;
    }

    llvm::Function *Parser::generateFunctionDefinition(MethodSymbol *symbol, const std::string &name) {
        std::vector<llvm::Type *> Doubles(symbol->args.size(), llvm::Type::getDoubleTy(*thisContext));
        if (!symbol->genericArgs.empty()) {
            return nullptr;
        }
        std::vector<llvm::Type *> args;
        args.reserve(symbol->args.size());
        for (const auto &item: symbol->args) {
            args.push_back(getLLVMType(item.returnType));
        }

        llvm::FunctionType *pFunctionType =
                llvm::FunctionType::get(llvm::Type::getDoubleTy(*thisContext), Doubles, false);

        llvm::Function *pFunction =
                llvm::Function::Create(pFunctionType, llvm::Function::ExternalLinkage, name, thisModule);

        unsigned Idx = 0;
        for (auto &Arg: pFunction->args()) Arg.setName(symbol->args[Idx++].name);

        return pFunction;
    }

    llvm::Type *Parser::getLLVMType(TraitSymbol *traitSymbol) const {
        std::map<TraitSymbol *, llvm::Type *> defaultType = {
                {BuiltinSymbols::Double, llvm::Type::getDoubleTy(*thisContext)},
                {BuiltinSymbols::Long, llvm::Type::getInt64Ty(*thisContext)},
                {BuiltinSymbols::Int, llvm::Type::getInt32Ty(*thisContext)},
                {BuiltinSymbols::Long, llvm::Type::getInt16PtrTy(*thisContext)},
        };
        if (defaultType.contains(traitSymbol)) return defaultType[traitSymbol];
        if (!traitSymbol->genericArgs.empty()) return nullptr;
        return nullptr;
    }

    void Parser::setupCodegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module) {
        this->thisContext = context;
        this->thisModule = module;
        this->irBuilder = builder;
        dynamic = llvm::StructType::create(*thisContext);
    }

    void Parser::generateFunctionIR(llvm::Function *symbol, Scope *definition) {}

    void Parser::importSymbols(const std::vector<std::string_view> &symbols) {}

    std::unique_ptr<AST::ASTNode> Parser::parseIf(Parser::token_iter &tokenIter) {
        if (tokenIter->content != "if" || tokenIter->type != Token::Name) {
            return nullptr;
        }
        tokenIter++;
        if (tokenIter == tokens.end()) {
            // todo: 分配错误码
            appendError(0, "unexpected EOF", tokens.back().stop);
            return nullptr;
        }
        bool hasBrackets = tokenIter->content == "(" && tokenIter->type == Token::Operator;
        auto node = std::make_unique<AST::IfElseNode>();
        if (!hasBrackets) {
            node->condition = parseExpression(tokenIter);
        } else {
            tokenIter++;
            node->condition = parseExpression(tokenIter, ')');
            if (tokenIter == tokens.end()) {
                appendError(0, "unexpected EOF", tokens.back().stop);
                return nullptr;
            }
        }
        if (node->condition == nullptr) {
            appendError(0, "expected expression.", tokenIter->start);
            return nullptr;
        }
        if (tokenIter == tokens.end()) {
            appendError(0, "unexpected EOF", tokens.back().stop);
            return nullptr;
        }
        bool hasBraces = tokenIter->content == "{" && tokenIter->type == Token::Operator;
        if (!hasBrackets && !hasBraces) {
            appendError(0, "Non-bracket if expected a '{'.", tokenIter->start);
            return nullptr;
        }
        node->thenBlock = parseBlockOrExpression(tokenIter);
        if (node->thenBlock == nullptr) {
            appendError(0, "expected expression.", tokenIter->start);
            return nullptr;
        }
        if (tokenIter == tokens.end()) {
            appendError(0, "unexpected EOF", tokenIter->stop);
            return nullptr;
        }
        if (tokenIter->type == Token::Name && tokenIter->content == "else") {
            tokenIter++;
            if (tokenIter == tokens.end()) {
                appendError(0, "unexpected EOF", tokenIter->stop);
                return nullptr;
            }
            node->elseBlock = parseBlockOrExpression(tokenIter);
            if (node->elseBlock == nullptr) {
                appendError(0, "expected expression.", tokenIter->start);
                return nullptr;
            }
        }
        return (node);
    }

    std::unique_ptr<AST::ASTNode> Parser::parseFor(Parser::token_iter &tokenIter) {
        return nullptr;
    }

    std::unique_ptr<AST::ASTNode> Parser::parseWhile(Parser::token_iter &tokenIter) {
        return nullptr;
    }

    std::unique_ptr<AST::ASTNode> Parser::parseBlockOrExpression(Parser::token_iter &tokenIter) {
        if (tokenIter->content != "{" || tokenIter->type != Token::Operator) {
            return parseExpression(tokenIter);
        }
        std::unique_ptr<AST::BlockNode> node =
                std::make_unique<AST::BlockNode>(token2scope[&*tokenIter]->getSignature());
        tokenIter++;
        while (tokenIter != tokens.end() && tokenIter->content != "}") {
            auto expr = parseExpression(tokenIter);
            if (expr == nullptr) {
                appendError(0, "expected expression.", tokenIter->start);
                return nullptr;
            }
            node->statements.push_back(std::move(expr));
        }
        tokenIter++;
        return node;
    }

    NamespaceSymbol *Parser::currentNamespaceSymbol(Symbol *symbol) {
        while (symbol != nullptr && symbol->type != Symbol::Method) {
            symbol = symbol->father;
        }
        return (NamespaceSymbol *)symbol;
    }

    void Parser::appendError(size_t code, const std::string &message, const SourcePosition &position,
                             const std::string &category) {
        errorTable.errors.push_back(std::make_unique<CompilerError>(position, code, message, category));
    }

    std::unique_ptr<AST::ASTNode> Parser::parseNext(token_iter &tokenIter) {
        if (tokenIter == tokens.end()) {
            appendError(0, "unexpected EOF", tokens.back().stop);
            return nullptr;
        }
        switch (tokenIter->type) {
            case Token::Name: {
                if (tokenIter->content == "if")
                    return parseIf(tokenIter);
                else if (tokenIter->content == "for")
                    return parseFor(tokenIter);
                else if (tokenIter->content == "while")
                    return parseWhile(tokenIter);
                else if (tokenIter->content == "return")
                    return parseReturn(tokenIter);
                else
                    return parseExpression(tokenIter);
            }
            case Token::Literal: {
                return nullptr;
            }
            case Token::Character:
            case Token::Number:
            case Token::Operator:
                return parseExpression(tokenIter);
            case Token::NewLine:
                return nullptr;
        }
    }

    std::unique_ptr<AST::ASTNode> Parser::parseReturn(Parser::token_iter &iter) {
        if (iter->content != "return" || iter->type != Token::Name) {
            return nullptr;
        }
        iter++;
        if (iter == tokens.end()) {
            appendError(0, "unexpected EOF", tokens.back().stop);
            return nullptr;
        }
        auto expr = parseExpression(iter);
        auto method = reference.find(&*iter);
        if (method == reference.end() || method->second->type != Symbol::Method) {
            appendError(0, "Function not found", tokens.back().stop);
            return nullptr;
        }
        auto node = std::make_unique<AST::ReturnNode>(std::move(expr), dynamic_cast<MethodSymbol *>(method->second));
        return node;
    }
}  // namespace Seserot
