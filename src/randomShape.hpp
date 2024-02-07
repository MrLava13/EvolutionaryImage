#pragma once

#include "shape.hpp"
#include "shapes/rectangle.hpp"
#include "shapes/triangle.hpp"
#include "shapes/circle.hpp"
#include "random.hpp"

shape *getRandomShape()
{
    // return new circle;
    // return new rectangle;
    //return new triangle;

    const uint8_t v = rando::randMaxInt<uint8_t>(2);

    //if (v == 2)
    //    return new circle;

    if (v == 1)
        return new rectangle;
    return new triangle;
}