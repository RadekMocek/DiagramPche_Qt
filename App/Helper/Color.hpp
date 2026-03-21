#pragma once

#include <tuple>

#include <QColor>

// Common colors
inline QColor COLOR_BLACK = QColor::fromRgb(0, 0, 0);
inline QColor COLOR_ERROR = QColor::fromRgb(211, 1, 2);
inline QColor COLOR_ERROR_HIGHLIGHT = QColor::fromRgb(211, 1, 2, 80);

// Color functions
using ColorTuple = std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;

inline QColor GetQColorFromTuple(const ColorTuple& tup)
{
    return QColor::fromRgb(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup), std::get<3>(tup));
}

inline ColorTuple GetColorTupleFromString(const std::string& color_str)
{
    if (color_str.length() == 9 && color_str[0] == '#') {
        unsigned int r = 0, g = 0, b = 0, a = 0;
        std::sscanf(color_str.c_str(), "#%2x%2x%2x%2x", &r, &g, &b, &a);
        return {
            static_cast<unsigned char>(r),
            static_cast<unsigned char>(g),
            static_cast<unsigned char>(b),
            static_cast<unsigned char>(a)
        };
    }
    return {0, 0, 0, 0};
}

inline QString GetQuotedRGBAHexFromQColor(const QColor& color)
{
    char result[12];
    std::sprintf(result, "\"#%02X%02X%02X%02X\"",
                 static_cast<unsigned char>(color.red()),
                 static_cast<unsigned char>(color.green()),
                 static_cast<unsigned char>(color.blue()),
                 static_cast<unsigned char>(color.alpha()));
    return result;
}

// === === === === === === === === === === === === === === === === === === === === === ===

inline void BenchmarkChangeColorImpl(unsigned char& r, unsigned char& g, unsigned char& b)
{
    if (r > 0) {
        r--;
    }
    else if (g > 0) {
        g--;
    }
    else if (b > 0) {
        b--;
    }
    else {
        r = 255;
        g = 255;
        b = 255;
    }
}

inline void BenchmarkChangeColor(unsigned char& r, unsigned char& g, unsigned char& b, const int modifier)
{
    switch (modifier) {
    default: return BenchmarkChangeColorImpl(r, g, b);
    case 1: return BenchmarkChangeColorImpl(r, b, g);
    case 2: return BenchmarkChangeColorImpl(g, r, b);
    case 3: return BenchmarkChangeColorImpl(g, b, r);
    case 4: return BenchmarkChangeColorImpl(b, r, g);
    case 5: return BenchmarkChangeColorImpl(b, g, r);
    }
}
