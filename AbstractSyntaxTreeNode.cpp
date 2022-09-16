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