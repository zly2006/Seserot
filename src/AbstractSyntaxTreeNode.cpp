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

#include "AbstractSyntaxTreeNode.h"
#include "Parser.h"

namespace Seserot {
    size_t AbstractSyntaxTreeNode::write(char *buffer) {
        assert(buffer != nullptr);
        static_assert(sizeof(action) == 4);
        static_assert(sizeof(long) == 8);
        memcpy(buffer, &action, 4);//len = 4
        memcpy(buffer + 4, &dataLength, 8);
        memcpy(buffer + 12, data, dataLength);
        uint64 tmp = children.size();
        memcpy(buffer + 12 + dataLength, &tmp, 8);//len = 12
        size_t len = 20 + dataLength + children.size() * 8;
        for (int i = 0; i < children.size(); ++i) {
            size_t added = children[i].write(buffer + len);
            memcpy(buffer + 20 + dataLength + i * 8, &added, 8);
            len += added;
        }
        return 0;
    }

    void AbstractSyntaxTreeNode::read(char *buffer, size_t len) {
        assert(buffer != nullptr);
        children.clear();
        memcpy(&action, buffer, 4);
        uint64 num;
        memcpy(&num, buffer + 4, 8);
        dataLength = num;
        data = new char[num];
        memcpy(data, buffer + 12, dataLength);
        memcpy(&num, buffer + 12 + dataLength, 8);
        uint64 offset = 20 + dataLength + num * 8;
        for (int i = 0; i < num; ++i) {
            uint64 childLen;
            memcpy(&childLen, buffer + 12 + i * 8, 8);
            children.emplace_back();
            children.back().read(buffer + offset, childLen);
            offset += childLen;
        }
        if (offset != len) {
            //todo: 运行时报错
        }
    }

    AbstractSyntaxTreeNode::~AbstractSyntaxTreeNode() {
        if (data != nullptr && dataLength != 0) {
            delete data;
        }
    }

    AbstractSyntaxTreeNode::AbstractSyntaxTreeNode(AbstractSyntaxTreeNode &&node) noexcept {
        data = node.data;
        dataLength = node.dataLength;
        action = node.action;
        children = std::vector(node.children);
        typeInferred = node.typeInferred;
        node.data = nullptr;
    }

    AbstractSyntaxTreeNode::AbstractSyntaxTreeNode(const AbstractSyntaxTreeNode &node) {
        data = new char[node.dataLength];
        dataLength = node.dataLength;
        action = node.action;
        children = std::vector(node.children);
        typeInferred = node.typeInferred;
        memcpy(data, node.data, node.dataLength);
    }

    AbstractSyntaxTreeNode &AbstractSyntaxTreeNode::operator=(const AbstractSyntaxTreeNode &node) {
        if (this == &node) {
            return *this;
        }
        data = new char[node.dataLength];
        dataLength = node.dataLength;
        action = node.action;
        children = std::vector(node.children);
        typeInferred = node.typeInferred;
        memcpy(data, node.data, node.dataLength);
        return *this;
    }

    llvm::Value *AbstractSyntaxTreeNode::CodeGen(Parser &parser) {
        // todo
        return nullptr;
    }

    AbstractSyntaxTreeNode::AbstractSyntaxTreeNode() = default;
} // Seserot