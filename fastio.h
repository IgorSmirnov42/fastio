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

template <typename T = int> inline T read();
template <typename T> inline void read(T &head);
template <typename T, typename ...Types> inline void read(T &head, Types &...tail);
template <typename T> inline std::vector<T> readVector(uint32_t n);
template <typename T> inline void readVector(std::vector<T> &vec);
template <int N> inline void write(const char (&arr)[N]);
template<typename T> inline void write(const T &elem);
template <typename T, typename ...Types> inline void write(const T &head, const Types &...tail);
template <typename T> inline void writeSep(char sep, const T &head);
template <typename T, typename ...Types> inline void writeSep(char sep, const T &head, const Types &...tail);
template <typename T> inline void writelnSep(char sep, const T &head);
template <typename T, typename ...Types> inline void writelnSep(char sep, const T &head, const Types &...tail);
template <typename T, typename C = int64_t> inline void writeFloatingPoint(const T &elem, size_t decimalPlaces = 6);
template <typename T> inline void writeVector(const std::vector<T> &vec, const std::string& sep = " ", const std::string& end = "\n");
template <typename ...Types> inline void writeln(const Types &...tail);

/** Read */

namespace read_private {

    static const int buf_size = 4096;
    static uint32_t current_pos = 0;
    static uint32_t current_len = 0;
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
        static char result[int(1e6 + 42)];
        int pos = 0;
        int chr = readChar();
        while (chr > 32) {
            result[pos++] = chr;
            chr = nextChar();
        }
        result[pos] = 0;
        return std::string(result);
    }

}

template<typename T>
inline T read() {
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
inline void read(T &head) {
    head = read<T>();
}

template <typename T, typename ...Types>
inline void read(T &head, Types &...tail) {
    head = read<T>();
    read(tail...);
}

template <typename T>
inline std::vector<T> readVector(uint32_t n) {
    std::vector<T> result(n);
    for (uint32_t i = 0; i < n; ++i) {
        result[i] = read<T>();
    }
    return result;
}

template <typename T>
inline void readVector(std::vector<T> &vec) {
    for (T &elem : vec) {
        elem = read<T>();
    }
}

/** Write */

namespace write_private {

    static const int buf_size = 4096;
    static uint32_t current_pos = 0;
    static unsigned char output_buf[buf_size];

    inline void writeBuffer() {
        uint32_t printed = 0;
        while (printed < current_pos) {
            printed += fwrite(output_buf, 1, current_pos - printed, stdout);
        }
        current_pos = 0;
    }

    inline void flush() {
        writeBuffer();
        fflush(stdout);
    }

    static struct flusher {
        ~flusher() {
            flush();
        }
    } flusher_;

    inline void writeChar(char c) {
        output_buf[current_pos++] = c;
        if (current_pos == buf_size) {
            writeBuffer();
        }
    }

    template<typename T>
    inline void writeNum(T num) {
        static_assert(std::is_integral<T>::value, "T must be integral");
        if (num == 0) {
            writeChar('0');
            return;
        }
        if (num < 0) {
            writeChar('-');
            num *= -1;
        }
        static char res[100];
        int pos = 0;
        while (num != 0) {
            res[pos++] = '0' + num % 10;
            num /= 10;
        }
        res[pos] = 0;
        for (int i = pos - 1; i >= 0; --i) {
            writeChar(res[i]);
        }
    }

    template<typename T, typename C = int>
    inline void writeFloatingPoint(T num, uint32_t decimalPlaces = 6) {
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
            for (uint32_t i = 0; i < decimalPlaces; ++i) {
                decimal *= 10;
                int dig = std::min(9, static_cast<int>(decimal));
                writeChar('0' + dig);
                decimal -= dig;
            }
        }
    }

    inline void writeString(const std::string &s) {
        for (char c : s) {
            writeChar(c);
        }
    }

}

template <int N>
inline void write(const char (&arr)[N]) {
    write_private::writeString(std::string(arr));
}

template<typename T>
inline void write(const T &elem) {
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
inline void write(const T &head, const Types &...tail) {
    write<T>(head);
    write(tail...);
}

template <typename T>
inline void writeSep(char sep, const T &head) {
    write<T>(head);
}

template <typename T, typename ...Types>
inline void writeSep(char sep, const T &head, const Types &...tail) {
    write<T>(head);
    write(sep);
    writeSep(sep, tail...);
}

template <typename T>
inline void writelnSep(char sep, const T &head) {
    write<T>(head);
    write('\n');
}

template <typename T, typename ...Types>
inline void writelnSep(char sep, const T &head, const Types &...tail) {
    write<T>(head);
    write(sep);
    writelnSep(sep, tail...);
}

template <typename T, typename C>
inline void writeFloatingPoint(const T &elem, uint32_t decimalPlaces) {
    static_assert(std::is_floating_point<T>::value, "T must be floating point");
    static_assert(std::is_integral<C>::value, "C must be integral");
    write_private::writeFloatingPoint<T, C>(elem, decimalPlaces);
}

template <typename T>
inline void writeVector(const std::vector<T> &vec, const std::string& sep, const std::string& end) {
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
inline void writeln(const Types &...tail) {
    write(tail...);
    write('\n');
}

#endif //FASTIO_FASTIO_H