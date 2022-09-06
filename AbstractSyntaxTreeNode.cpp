//
// Created by 赵李言 on 2022/9/2.
//

#include "AbstractSyntaxTreeNode.h"

namespace Seserot {
    size_t AbstractSyntaxTreeNode::write(char *buffer) {
        static_assert(sizeof(action) == 4);
        memcpy(buffer, &action, 4);
        unsigned int tmp = children.size();
        memcpy(buffer + 4, &tmp, 4);
        size_t len = 8 + children.size() * 4;
        for (int i = 0; i < children.size(); ++i) {
            size_t added = children[i].write(buffer + len);
            memcpy(buffer + 8 + i * 4, &added, 4);
            len += added;
        }
        return 0;
    }

    void AbstractSyntaxTreeNode::read(char *buffer, size_t len) {
        children.clear();
        memcpy(&action, buffer, 4);
        size_t num;
        memcpy(&num, buffer + 4, 4);
        size_t offset = 8 + num * 4;
        for (int i = 0; i < num; ++i) {
            unsigned int len;
            memcpy(&len, buffer + 8 + i * 4, 4);
            children.emplace_back();
            children.back().read(buffer + offset, len);
            offset += len;
        }
        if (offset != len) {
            //todo
        }
    }
} // Seserot