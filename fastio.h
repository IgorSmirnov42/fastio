#ifndef FASTIO_FASTIO_H
#define FASTIO_FASTIO_H

// Fast input-output
// Author: Igor Smirnov

#include <type_traits>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

/** Interface */

template <typename T = int> T read();
template <typename T> void read(T &head);
template <typename T, typename ...Types> void read(T &head, Types &...tail);
template <typename T> std::vector<T> readVector(size_t n);
template <typename T> void readVector(std::vector<T> &vec);
template <int N> void write(const char (&arr)[N]);
template<typename T> void write(const T &elem);
template <typename T, typename ...Types> void write(const T &head, const Types &...tail);
template <typename T> void writeSep(char sep, const T &head);
template <typename T, typename ...Types> void writeSep(char sep, const T &head, const Types &...tail);
template <typename T> void writelnSep(char sep, const T &head);
template <typename T, typename ...Types> void writelnSep(char sep, const T &head, const Types &...tail);
template <typename T, typename C = int64_t> void writeFloatingPoint(const T &elem, size_t decimalPlaces = 6);
template <typename T> void writeVector(const std::vector<T> &vec, const std::string& sep = " ", const std::string& end = "\n");
template <typename ...Types> void writeln(const Types &...tail);

/** Read */

namespace read_private {

    static const int buf_size = 4096;
    static size_t current_pos = 0;
    static size_t current_len = 0;
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
    inline std::pair<int, T> readNumPart() {
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
        return {sgn, res};
    }

    template<typename T>
    inline T readNum() {
        static_assert(std::is_integral<T>::value, "T must be integral");
        auto [sgn, numPart] = readNumPart<T>();
        return sgn * numPart;
    }

    template<typename T>
    inline T readFloatingPoint() {
        static_assert(std::is_floating_point<T>::value, "T must be floating point");
        auto [sgn, res] = readNumPart<T>();
        if (lastChar == '.') {
            T coef = 1e-1;
            int digit = nextChar();
            while ('0' <= digit && digit <= '9') {
                res += (digit - '0') * coef;
                coef *= 1e-1;
                digit = nextChar();
            }
        }
        return res * sgn;
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

template<typename T>
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

/** Write */

namespace write_private {

    static const int buf_size = 4096;
    static size_t current_pos = 0;
    static unsigned char output_buf[buf_size];

    void writeBuffer() {
        size_t printed = 0;
        while (printed < current_pos) {
            printed += fwrite(output_buf, 1, current_pos - printed, stdout);
        }
        current_pos = 0;
    }

    void flush() {
        writeBuffer();
        fflush(stdout);
    }

    static struct flusher {
        ~flusher() {
            flush();
        }
    } flusher_;

    void writeChar(char c) {
        output_buf[current_pos++] = c;
        if (current_pos == buf_size) {
            writeBuffer();
        }
    }

    template<typename T>
    void writeNum(T num) {
        static_assert(std::is_integral<T>::value, "T must be integral");
        if (num == 0) {
            writeChar('0');
            return;
        }
        if (num < 0) {
            writeChar('-');
            num *= -1;
        }
        std::vector<char> result;
        result.reserve(20);
        while (num != 0) {
            result.push_back('0' + num % 10);
            num /= 10;
        }
        for (size_t i = result.size(); i >= 1; --i) {
            writeChar(result[i - 1]);
        }
    }

    template<typename T, typename C = int>
    void writeFloatingPoint(T num, size_t decimalPlaces = 6) {
        static_assert(std::is_floating_point<T>::value, "T must be floating point");
        static_assert(std::is_integral<C>::value, "C must be integral");
        if (num < 0) {
            num *= -1;
            writeChar('-');
        }
        C intPart = static_cast<C>(num);
        writeNum(intPart);
        if (decimalPlaces > 0) {
            writeChar('.');
            T decimal = num - intPart;
            for (size_t i = 0; i < decimalPlaces; ++i) {
                decimal *= 10;
                int dig = std::min(9, static_cast<int>(decimal));
                writeChar('0' + dig);
                decimal -= dig;
            }
        }
    }

    void writeString(const std::string &s) {
        for (char c : s) {
            writeChar(c);
        }
    }

}


template <int N>
void write(const char (&arr)[N]) {
    write_private::writeString(std::string(arr));
}

template<typename T>
void write(const T &elem) {
    if constexpr (std::is_same<T, char>::value) {
        write_private::writeChar(elem);
        return;
    }
    else if constexpr (std::is_integral<T>::value) {
        write_private::writeNum(elem);
        return;
    }
    else if constexpr (std::is_floating_point<T>::value) {
        write_private::writeFloatingPoint(elem);
        return;
    }
    else {
        write_private::writeString(elem);
        return;
    }
}

template <typename T, typename ...Types>
void write(const T &head, const Types &...tail) {
    write<T>(head);
    write(tail...);
}

template <typename T>
void writeSep(char sep, const T &head) {
    write<T>(head);
}

template <typename T, typename ...Types>
void writeSep(char sep, const T &head, const Types &...tail) {
    write<T>(head);
    write(sep);
    writeSep(sep, tail...);
}

template <typename T>
void writelnSep(char sep, const T &head) {
    write<T>(head);
    write('\n');
}

template <typename T, typename ...Types>
void writelnSep(char sep, const T &head, const Types &...tail) {
    write<T>(head);
    write(sep);
    writelnSep(sep, tail...);
}

template <typename T, typename C>
void writeFloatingPoint(const T &elem, size_t decimalPlaces) {
    static_assert(std::is_floating_point<T>::value, "T must be floating point");
    static_assert(std::is_integral<C>::value, "C must be integral");
    write_private::writeFloatingPoint<T, C>(elem, decimalPlaces);
}

template <typename T>
void writeVector(const std::vector<T> &vec, const std::string& sep, const std::string& end) {
    bool isFirst = true;
    for (const T &elem : vec) {
        if (!isFirst) {
            write(sep);
        }
        write(elem);
        isFirst = false;
    }
    write(end);
}

template <typename ...Types>
void writeln(const Types &...tail) {
    write(tail...);
    write('\n');
}

#endif //FASTIO_FASTIO_H