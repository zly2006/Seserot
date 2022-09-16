//
// Created by 赵李言 on 2022/8/12.
//

#ifndef SESEROT_GEN0_BUILDIN_H
#define SESEROT_GEN0_BUILDIN_H
#include "Symbol.h"
namespace Seserot {

    class BuildIn {
    public:
        ClassSymbol *const voidClass = new ClassSymbol(
                nullptr,
                "Void",
                {},
                nullptr, Static | ValueType);
        ClassSymbol *const intClass = new ClassSymbol(
                nullptr,
                "Int",
                {},
                nullptr, Static | ValueType);
        ClassSymbol *const longClass = new ClassSymbol(
                nullptr,
                "Long",
                {},
                nullptr, Static | ValueType);
        ClassSymbol *const floatClass = new ClassSymbol(
                nullptr,
                "Float",
                {},
                nullptr, Static | ValueType);
        ClassSymbol *const doubleClass = new ClassSymbol(
                nullptr,
                "Double",
                {},
                nullptr, Static | ValueType);
        ClassSymbol *const stringClass = new ClassSymbol(
                nullptr,
                "String",
                {},
                nullptr, Static);
        ClassSymbol *const classClass = new ClassSymbol(
                nullptr,
                "Class",
                {},
                nullptr, Static);
        ClassSymbol *const functionClass = new ClassSymbol(
                nullptr,
                "Function",
                {
                        ClassSymbol(nullptr, "P", {}, nullptr, None),
                        ClassSymbol(nullptr, "T", {}, nullptr, None),
                        ClassSymbol(nullptr, "...", {}, nullptr, None),
                },
                nullptr, Static | ValueType);

        TraitSymbol *const numberTrait = new TraitSymbol(
                nullptr,
                "Number",
                (Modifiers) (Static | ValueType)
        );
    };

} // Seserot

#endif //SESEROT_GEN0_BUILDIN_H
