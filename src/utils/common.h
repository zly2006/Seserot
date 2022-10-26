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

#ifndef SESEROT_GEN0_COMMON_H
#define SESEROT_GEN0_COMMON_H

#include <functional>
#include <string>
#include <vector>

namespace Seserot::utils {
    template <class T, class P, class Container = std::vector<T>, class OutputContainer = std::vector<P>>
    OutputContainer map(typename Container::itertor first, typename Container::itertor last, std::function<P(T)> func) {
        OutputContainer result;
        for (auto it = first; it != last; ++it) {
            result.push_back(func(*it));
        }
        return result;
    }
    template <class T>
    class return_type {
       public:
        using type = void;
    };
    template <class T, class... P>
    class return_type<T(P...)> {
       public:
        using type = T;
    };
}  // namespace Seserot::utils
#endif  // SESEROT_GEN0_COMMON_H
