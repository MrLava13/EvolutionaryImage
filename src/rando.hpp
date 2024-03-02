#pragma once

#include <cstdlib>
#include <random>
#include "point.hpp"
#include "color.hpp"
#include "bounds.hpp"

namespace rando
{
    std::minstd_rand RAND_G;
    template <class T>
    constexpr T RAND_DIV = 1.0f / RAND_G.max();

    inline int rand() { return RAND_G(); }

    template <class T>
    inline T randMinMax(T a, T b) { return ((float)(b - a)) * ((T)(((float)rand()) * RAND_DIV<float>)) + a; }
    inline float randMinMax(const point2f &p) { return randMinMax(p.x, p.y); }
    template <class T>
    inline T randMaxInt(T a) { return rand() % a; }

    inline bool randBool() { return 1 == (rand() % 2); }

    inline color getRandColor() { return {(uint8_t)randMaxInt(255), (uint8_t)randMaxInt(255), (uint8_t)randMaxInt(255)}; }

    inline point2 randPoint2(const bounds &b)
    {
        return point2(rando::randMinMax(b.min.x, b.max.x), rando::randMinMax(b.min.y, b.max.y));
    }

    int32_t RAND_ORIGINAL_CHANCE = 4;
    bounds RAND_POINT_BOUNDS = {0.9, 1.1, 0.9, 1.1};
    point2f RAND_COLOR_BOUNDS = {0.8, 1.2};
    point2f RAND_FLOAT_BOUNDS = {0.95, 1.05};

    template <class T>
    inline base_point<T> rand(const base_point<T> &v, const bounds b = RAND_POINT_BOUNDS)
    {
        return randMaxInt(RAND_ORIGINAL_CHANCE) != 0
                   ? base_point<T>(
                         (float)v.x * randMinMax(b.min.x, b.max.x),
                         (float)v.y * randMinMax(b.min.y, b.max.y))
                   : v;
    }
    template <class T>
    inline base_point<T> rand(const base_point<T> &v1, const base_point<T> &v2, const bounds b = RAND_POINT_BOUNDS)
    {
        return rand(randBool() ? v1 : v2, b);
    }
    inline point2f rand(const point2f &v, const bounds b = RAND_POINT_BOUNDS)
    {
        return randMaxInt(RAND_ORIGINAL_CHANCE) != 0
                   ? point2f(
                         v.x * randMinMax(b.min.x, b.max.x),
                         v.y * randMinMax(b.min.y, b.max.y))
                   : v;
    }
    inline color rand(const color &c, const point2f l = RAND_COLOR_BOUNDS)
    {
        return randMaxInt(RAND_ORIGINAL_CHANCE) != 0
                   ? color(
                         (uint8_t)((float)c.r * randMinMax(l)),
                         (uint8_t)((float)c.g * randMinMax(l)),
                         (uint8_t)((float)c.b * randMinMax(l)))
                   : c;
    }
    inline color rand(const color &c1, const color &c2, const point2f l = RAND_COLOR_BOUNDS)
    {
        return rand(randBool() ? c1 : c2, l);
    }
    inline float rand(const float &f, const point2f b = RAND_FLOAT_BOUNDS)
    {
        return randMaxInt(RAND_ORIGINAL_CHANCE) != 0 ? f * randMinMax(b) : f;
    }
    inline float rand(const float &f1, const float &f2, const point2f b = RAND_FLOAT_BOUNDS)
    {
        return rand(randBool() ? f1 : f2, b);
    }
};