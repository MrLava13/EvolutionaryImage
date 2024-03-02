#pragma once

#include "shape.hpp"
#include "shapes/rectangle.hpp"
#include "shapes/triangle.hpp"
#include "shapes/circle.hpp"
#include "rando.hpp"

inline shape *getRandomShape()
{
    // return new circle;
    // return new rectangle;
    // return new triangle;

    uint8_t v = rando::randMaxInt(2);

    // if (v == 2)
    //     return new circle;

    if (v == 1)
        return new rectangle;
    return new triangle;
}