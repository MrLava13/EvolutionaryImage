#pragma once
#include <cstdint>
#include <cmath>
#include <iostream>
struct color
{
    uint8_t R = 0, G = 0, B = 0, A = 0;
    color() {}
    color(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255)
        : R(R), G(G), B(B), A(A) {}
    color(const color &c)
        : R(c.R), G(c.G), B(c.B), A(c.A) {}

    inline float findColorDifference(const color &two) const
    {
        return std::abs(R - two.R) + std::abs(G - two.G) + std::abs(B - two.B) + std::abs(A - two.A);
        /*
        float rt = ((float)R) - ((float)two.R),
              gt = ((float)G) - ((float)two.G),
              bt = ((float)B) - ((float)two.B),
              at = ((float)A) - ((float)two.A);

        return std::sqrt((rt * rt) + (gt * gt) + (bt * bt) + (at * at)); */
    }

    color LerpRGB(color a, color b, float t)
    {
        return color(
            a.R + (b.R - a.R) * t,
            a.G + (b.G - a.G) * t,
            a.B + (b.B - a.B) * t,
            a.A + (b.A - a.A) * t);
    }

    bool operator==(const color &c) const { return R == c.R && G == c.G && B == c.B && A == c.A; }
    bool operator!=(const color &c) const { return R != c.R || G != c.G || B != c.B || A != c.A; }

    color &operator=(const color &c)
    {
        R = c.R;
        G = c.G;
        B = c.B;
        A = c.A;
        return *this;
    }

    // constexpr static color WHITE = color(255, 255, 255);
};

std::ostream &operator<<(std::ostream &os, const color &c) { return os << '(' << (int)c.R << ", " << (int)c.G << ", " << (int)c.B << ", " << (int)c.A << ')'; }

union colorInt
{
    color c;
    int32_t i;

    colorInt() : c() {}
    colorInt(const color &co) : c(co) {}
    colorInt(const colorInt &cc) : i(cc.i) {}
    // colorInt(const colorInt &&cc) = default;
    colorInt(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255)
        : c(R, G, B, A) {}

    colorInt &operator=(const color &col)
    {
        c = col;
        return *this;
    }
    colorInt &operator=(const colorInt &col)
    {
        i = col.i;
        return *this;
    }
};