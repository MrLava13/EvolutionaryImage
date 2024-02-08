#pragma once
#include "../shape.hpp"

class rectangle : public shape
{
private:
    float angle;
    point2f center, size;
    color c;

public:
    rectangle(float angle, point2f center, point2f size, color c)
        : angle(angle), center(center), size(size), c(c) {}
    rectangle() {}

    void setRandom(bounds b, [[maybe_unused]] const image *im)
    {
        // bounds tmp;
        angle = Ran.randMinMax(-3.14159, 3.14159);
        size = {Ran.randMinMax(3.0f, b.max.x), Ran.randMinMax(3.0f, b.max.y)};
        center = {Ran.randMinMax(size.x * 0.5f + b.min.x, b.max.x - size.x * 0.5f), Ran.randMinMax(size.y * 0.5f + b.min.y, b.max.y - size.y * 0.5f)};
        //    tmp = getBounds();
        // c = im->getClampedPixel(center);
        //  c = im->getColorAv<7>(center);
        c = Ran.getRandColor();
    }

    void addShape(image *v) const
    {
        point2f halfSize = size / 2;
        point2 p1 = (center + point2f(-halfSize.x, -halfSize.y)).rotate(center, angle),
               p2 = (center + point2f(halfSize.x, -halfSize.y)).rotate(center, angle),
               p3 = (center + point2f(halfSize.x, halfSize.y)).rotate(center, angle),
               p4 = (center + point2f(-halfSize.x, halfSize.y)).rotate(center, angle);
        v->drawTriangle(p1, p2, p3, c);
        v->drawTriangle(p1, p3, p4, c);
        // v->bresenhamLine(p1, p3, c);
    }

    bounds getBounds() const
    {
        point2f halfSize = size / 2;
        point2 p1 = (center + point2f(-halfSize.x, -halfSize.y)).rotate(center, angle),
               p2 = (center + point2f(halfSize.x, -halfSize.y)).rotate(center, angle),
               p3 = (center + point2f(halfSize.x, halfSize.y)).rotate(center, angle),
               p4 = (center + point2f(-halfSize.x, halfSize.y)).rotate(center, angle);
        return {
            p1.min(p2.min(p3.min(p4))),
            p1.max(p2.max(p3.max(p4)))};
    }

    shape *genFromSelf() const { return new rectangle(Ran(angle), Ran(center), Ran(size), Ran(c)); }

    shape *clone() const { return new rectangle(angle, center, size, c); }

    void scale(point2f s) { size *= s; }
};
