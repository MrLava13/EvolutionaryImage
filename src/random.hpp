#pragma once

#include <cstdlib>
#include "point.hpp"
#include "color.hpp"

namespace rando
{
    constexpr static float RAND_DIV = 1.0f / ((float)RAND_MAX);
    template <class T>
    inline static T randMinMax(T a, T b) { return (b - a) * ((T)(((float)std::rand()) * RAND_DIV)) + a; }
    inline static float randMinMax(const point2f &p) { return randMinMax(p.x, p.y); }
    template <class T>
    inline static T randMaxInt(T a) { return std::rand() % a; }

    inline static bool randBool() { return 1 == (std::rand() % 2); }

    inline float getRand() { return randMinMax(0.95f, 1.05f); }

    inline color getRandColor() { return {(uint8_t)randMaxInt(255), (uint8_t)randMaxInt(255), (uint8_t)randMaxInt(255)}; }

    template <class T>
    base_point<T> rand(const base_point<T> &v, const bounds b = bounds(0.9, 1.1, 0.9, 1.1))
    {
        return randBool() ? base_point<T>{
            (T)((float)v.x * randMinMax(b.min.x, b.max.x)),
            (T)((float)v.y * randMinMax(b.min.y, b.max.y))} : v;
    }
    color rand(const color &c, const point2f l = point2f(0.8, 1.2))
    {
        return randBool() ? color{
            (uint8_t)((float)c.r * randMinMax(l)),
            (uint8_t)((float)c.g * randMinMax(l)),
            (uint8_t)((float)c.b * randMinMax(l))} : c;
    }
    float rand(const float &f)
    {
        return randBool() ? f * getRand(): f;
    }
};