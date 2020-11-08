#ifndef FASTIO_FASTIO_H
#define FASTIO_FASTIO_H

// Fast input-output
// Author: Igor Smirnov

#include <type_traits>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

/** Read */

namespace read_private {

    static const int buf_size = 4096;
    static int current_pos = 0;
    static int current_len = 0;
    static int lastChar = 0;
    static unsigned char input_buf[buf_size];

    inline int nextChar() {
        ++current_pos;
        if (current_pos >= current_len) {
            current_len = fread(input_buf, 1, buf_size, stdin);
            current_pos = 0;
            if (current_len == 0) {
                return lastChar = -1;
            }
        }
        return lastChar = input_buf[current_pos];
    }

    inline void skipSpaceChars() {
        int c = nextChar();
        while (c <= 32 && c != -1) {
            c = nextChar();
        }
    }

    inline int readChar() {
        skipSpaceChars();
        return lastChar;
    }

    template<typename T>
    inline T readNumPart() {
        static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value);
        int signOrFirstDigit = readChar();
        int sgn = 1;
        if (signOrFirstDigit == '-' || signOrFirstDigit == '+') {
            if (signOrFirstDigit == '-') {
                sgn = -1;
            }
            signOrFirstDigit = readChar();
        }
        int digit = signOrFirstDigit;
        T res = digit - '0';
        digit = nextChar();
        while ('0' <= digit && digit <= '9') {
            res = res * 10 + (digit - '0');
            digit = nextChar();
        }
        return res * sgn;
    }

    template<typename T>
    inline T readNum() {
        static_assert(std::is_integral<T>::value, "T must be integral");
        return readNumPart<T>();
    }

    template<typename T>
    inline T readFloatingPoint() {
        static_assert(std::is_floating_point<T>::value, "T must be floating point");
        T res = readNumPart<T>();
        if (lastChar == '.') {
            T coef = 1e-1;
            int digit = nextChar();
            while ('0' <= digit && digit <= '9') {
                res += (digit - '0') * coef;
                coef *= 1e-1;
                digit = nextChar();
            }
        }
        return res;
    }

    inline std::string readString() {
        std::vector<char> result;
        int chr = readChar();
        while (chr > 32) {
            result.push_back(chr);
            chr = nextChar();
        }
        return std::string(result.begin(), result.end());
    }

}

template<typename T = int>
T read() {
    if constexpr (std::is_same<T, char>::value) {
        return read_private::readChar();
    }
    else if constexpr (std::is_integral<T>::value) {
        return read_private::readNum<T>();
    }
    else if constexpr (std::is_floating_point<T>::value) {
        return read_private::readFloatingPoint<T>();
    }
    else if constexpr (std::is_same<T, std::string>::value) {
        return read_private::readString();
    }
}

template <typename T>
void read(T &head) {
    head = read<T>();
}

template <typename T, typename ...Types>
void read(T &head, Types &...tail) {
    head = read<T>();
    read(tail...);
}

template <typename T>
std::vector<T> readVector(size_t n) {
    std::vector<T> result(n);
    for (size_t i = 0; i < n; ++i) {
        result[i] = read<T>();
    }
    return result;
}

template <typename T>
void readVector(std::vector<T> &vec) {
    for (T &elem : vec) {
        elem = read<T>();
    }
}

#endif //FASTIO_FASTIO_H
