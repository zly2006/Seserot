//
// Created by 赵李言 on 2022/8/12.
//

#ifndef SESEROT_GEN0_BUILDIN_H
#define SESEROT_GEN0_BUILDIN_H
#include "Symbol.h"
namespace Seserot {

    class BuildIn {
    public:
        const ClassSymbol *const voidClass = new ClassSymbol(
                nullptr,
                "Void",
                {},
                false,
                nullptr,
                true);
        const ClassSymbol *const intClass = new ClassSymbol(
                nullptr,
                "Int",
                {},
                false,
                nullptr,
                true);
        const ClassSymbol *const longClass = new ClassSymbol(
                nullptr,
                "Long",
                {},
                false,
                nullptr,
                true);
        const ClassSymbol *const floatClass = new ClassSymbol(
                nullptr,
                "Float",
                {},
                false,
                nullptr,
                true);
        const ClassSymbol *const doubleClass = new ClassSymbol(
                nullptr,
                "Double",
                {},
                false,
                nullptr,
                true);
    };

} // Seserot

#endif //SESEROT_GEN0_BUILDIN_H
