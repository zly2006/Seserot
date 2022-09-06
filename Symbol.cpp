//
// Created by 赵李言 on 2022/9/6.
//
#include "Symbol.h"

bool Seserot::isPrivateModifier(Modifiers modifiers) {
    return (modifiers & PrivateProtected) && (modifiers & PrivateInternal);
}

bool Seserot::isPublicModifier(Seserot::Modifiers modifiers) {
    return !(modifiers & PrivateProtected) && !(modifiers & PrivateInternal);
}

bool Seserot::isProtectedModifier(Seserot::Modifiers modifiers) {
    return (modifiers & PrivateProtected) && !(modifiers & PrivateInternal);
}

bool Seserot::isInternalModifier(Seserot::Modifiers modifiers) {
    return !(modifiers & PrivateProtected) && (modifiers & PrivateInternal);
}

