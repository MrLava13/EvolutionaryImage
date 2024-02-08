#pragma once

#include <cstdlib>
#include "point.hpp"
#include "color.hpp"

class rando
{
public:
    constexpr static float RAND_DIV = 1.0f / ((float)RAND_MAX);
    template <class T>
    inline static T randMinMax(T a, T b) { return (b - a) * ((T)(((float)std::rand()) * RAND_DIV)) + a; }
    inline static float randMinMax(const point2f &p) { return randMinMax(p.x, p.y); }
    template <class T>
    inline static T randMaxInt(T a) { return std::rand() % a; }

    inline static bool randBool() { return 1 == (std::rand() % 2); }

public:
    rando() {}

    inline float getRand() const { return randMinMax(0.95f, 1.05f); }

    inline color getRandColor() const { return {(uint8_t)randMaxInt(255), (uint8_t)randMaxInt(255), (uint8_t)randMaxInt(255)}; }

    template <class T>
    base_point<T> operator()(const base_point<T> &v, const bounds b = bounds(0.9, 1.1, 0.9, 1.1)) const
    {
        return randBool() ? base_point<T>{
            (T)((float)v.x * randMinMax(b.min.x, b.max.x)),
            (T)((float)v.y * randMinMax(b.min.y, b.max.y))} : v;
    }
    color operator()(const color &c, const point2f l = point2f(0.8, 1.2)) const
    {
        return randBool() ? color{
            (uint8_t)((float)c.R * randMinMax(l)),
            (uint8_t)((float)c.G * randMinMax(l)),
            (uint8_t)((float)c.B * randMinMax(l))} : c;
    }
    float operator()(const float &f) const
    {
        return randBool() ? f * getRand(): f;
    }
};