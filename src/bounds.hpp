#pragma once
#include <assert.h>
#include "point.hpp"

struct bounds
{
    point2f min, max;

    bounds() {}
    bounds(float minX, float maxX, float minY, float maxY)
        : min(minX, minY), max(maxX, maxY)
    {
        assert(minX < maxX);
        assert(minY < maxY);
    }
    bounds(const point2f &min_, const point2f &max_)
        : min(min_), max(max_) {}
    bounds(const bounds &b)
        : min(b.min), max(b.max) {}
    bounds(bounds &&b) noexcept
    {
        std::swap(min, b.min);
        std::swap(max, b.max);
    }
    bounds &operator=(bounds &&b) noexcept
    {
        if (this != &b)
        {
            std::swap(min, b.min);
            std::swap(max, b.max);
        }
        return *this;
    }
    bounds &operator=(const bounds &v)
    {
        min = v.min;
        max = v.max;

        return *this;
    }

    bounds clamp(const bounds &b) const { return {min.clamp(b.min, b.max), max.clamp(b.min, b.max)}; }

    bounds expand(float size) const { return {min - size, max + size}; }
    bounds expandX(float size) const { return {{min.x - size, min.y}, {max.x + size, max.y}}; }
    bounds expandY(float size) const { return {{min.x, min.y + size}, {max.x, max.y + size}}; }

    bool isIn(const point2f &p) const { return (p.x <= max.x || p.x >= min.x) || (p.y <= max.y || p.y >= min.y); }
    bool isNotIn(const point2f &p) const { return (p.x > max.x || p.x < min.x) && (p.y > max.y || p.y < min.y); }

    bool operator==(const bounds &b) const { return min == b.min && max == b.max; }
    bool operator!=(const bounds &b) const { return min != b.min || max != b.max; }
};
