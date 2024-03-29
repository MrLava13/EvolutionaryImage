#pragma once
#include <cstdint>
#include <cmath>
#include <iostream>
#include <fstream>
#include <utility>

struct color
{
    uint8_t r = 0, g = 0, b = 0, a = 0;

    color() {}
    color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}

    color(const color &c) : r(c.r), g(c.g), b(c.b), a(c.a) {}
    color(color &&c) noexcept : r(std::exchange(c.r, 0)), g(std::exchange(c.g, 0)), b(std::exchange(c.b, 0)), a(std::exchange(c.a, 0)) {}

    color &operator=(const color &c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        return *this;
    }
    color &operator=(color &&c) noexcept
    {
        if (this != &c)
        {
            r = std::exchange(c.r, 0);
            g = std::exchange(c.g, 0);
            b = std::exchange(c.b, 0);
            a = std::exchange(c.a, 0);
        }
        return *this;
    }

    inline float findColorDifference(const color &two) const
    {
        return std::abs(r - two.r) + std::abs(g - two.g) + std::abs(b - two.b) + std::abs(a - two.a);
    }

    color LerpRGB(color a_, color b_, float t)
    {
        return color(
            a_.r + (b_.r - a_.r) * t,
            a_.g + (b_.g - a_.g) * t,
            a_.b + (b_.b - a_.b) * t,
            a_.a + (b_.a - a_.a) * t);
    }

    void write(std::ofstream &out) const
    {
        out.write((const char *)&r, sizeof(uint8_t));
        out.write((const char *)&g, sizeof(uint8_t));
        out.write((const char *)&b, sizeof(uint8_t));
        out.write((const char *)&a, sizeof(uint8_t));
    }
    void read(std::ifstream &in)
    {
        in.read((char *)&r, sizeof(uint8_t));
        in.read((char *)&g, sizeof(uint8_t));
        in.read((char *)&b, sizeof(uint8_t));
        in.read((char *)&a, sizeof(uint8_t));
    }

    bool operator==(const color &c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
    bool operator!=(const color &c) const { return r != c.r || g != c.g || b != c.b || a != c.a; }
};

std::ostream &operator<<(std::ostream &os, const color &c)
{
    return os << '(' << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << ", " << (int)c.a << ')';
}

union colorInt
{
    color c;
    int32_t i;

    colorInt() : c() {}
    colorInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : c(r, g, b, a) {}

    colorInt(const color &co) : c(co) {}
    colorInt(const colorInt &cc) : i(cc.i) {}
    colorInt(colorInt &&cc) noexcept : i(std::exchange(cc.i, 0)) {}

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
    colorInt &operator=(colorInt &&cc) noexcept
    {
        i = std::exchange(cc.i, 0);
        return *this;
    }
};