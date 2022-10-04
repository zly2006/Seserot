#include <string>
#include <sstream>

#ifndef SESEROT_UTILS_SUM_STRING_H
#define SESEROT_UTILS_SUM_STRING_H

template<class T>
std::string toString(T t) {
    std::stringstream ss;
    ss << t;
    std::string ret;
    ss >> ret;
    return ret;
}

template<typename T>
inline std::string sum(T t) {
    return toString(t);
}

template<typename T, typename ...P>
inline std::string sum(T t, P...p) {
    return toString(t) + sum(p...);
}

#endif