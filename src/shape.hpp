#pragma once
#include "bounds.hpp"
#include "image.hpp"
#include "rando.hpp"

enum class shapeType : char
{
    triangle,
    rectangle,
    circle
};

class shape
{
public:
    virtual ~shape() {}

    virtual void setRandom(const bounds &b, const image *im) = 0;

    virtual void addShape(image *v) const = 0;

    virtual const color &getColor() const = 0;
    virtual bounds getBounds() const = 0;

    virtual shape *regenerate() const = 0;
    virtual shape *regenerate(const color &c) const = 0;
    virtual shape *regenerate(const shape *s) const = 0;
    virtual shape *clone() const = 0;

    virtual void scale(point2f s) = 0;

    virtual void write(std::ofstream &out) const = 0;
    virtual void read(std::ifstream &in) = 0;

    virtual shapeType getShapeType() const = 0;
};
