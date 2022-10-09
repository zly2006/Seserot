#ifndef SESEROT_GEN0_MYFORMAT_H
#define SESEROT_GEN0_MYFORMAT_H

#include <utility>
template <size_t Size, char ... Chars>
struct MyFormat {
    static constexpr size_t size = Size;
    static constexpr char data[Size] = {Chars...};
    static constexpr char chars[Size + 1] = {Chars..., '\0'};
};

template <typename FirstArg, typename ...Args>
std::string format(std::string_view fmt, const FirstArg& firstArg, const Args&... args) {
    if (fmt[0] == '{' && fmt[1] == '}') {
        return std::to_string(args...);
    }
}

template <typename FirstArg>
std::string format(std::string_view fmt, const FirstArg& firstArg) {
    if (fmt[0] == '{' && fmt[1] == '}') {
        return std::to_string(firstArg);
    }
}

#endif //SESEROT_GEN0_MYFORMAT_H
