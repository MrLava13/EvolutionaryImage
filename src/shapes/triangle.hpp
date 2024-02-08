#pragma once
#include "../shape.hpp"

class triangle : public shape
{
private:
    point2f p1, p2, p3;
    color c;

public:
    triangle() {}
    triangle(point2f p1, point2f p2, point2f p3, color c)
        : p1(p1), p2(p2), p3(p3), c(c) {}
    ~triangle() {}
    void setRandom(bounds bound, const image *im)
    {
        do
        {
            p1 = {Ran.randMinMax(bound.min.x, bound.max.x), Ran.randMinMax(bound.min.y, bound.max.y)};
            p2 = {Ran.randMinMax(bound.min.x, bound.max.x), Ran.randMinMax(bound.min.y, bound.max.y)};
            p3 = {Ran.randMinMax(bound.min.x, bound.max.x), Ran.randMinMax(bound.min.y, bound.max.y)};
            //c = im->getColorAv<3>({(int32_t)((p1.x + p2.x + p3.x) / 3), (int32_t)((p1.y + p2.y + p3.y) / 3)});
            //c = im->getClampedPixel({(int32_t)((p1.x + p2.x + p3.x) / 3), (int32_t)((p1.y + p2.y + p3.y) / 3)});
            // if (c.A == 0)
            c = Ran.getRandColor();
        } while (im->getBounds().isNotIn(p1) && im->getBounds().isNotIn(p2) && im->getBounds().isNotIn(p3)); // Verify at least one of the vertacies are in the image
    }
    void addShape(image *v) const
    {
        v->drawTriangle(p1, p2, p3, c);
    }

    bounds getBounds() const
    {
        return {
            p1.min(p2.min(p3)),
            p1.max(p2.max(p3))};
    }

    shape *genFromSelf() const { return new triangle(Ran(p1), Ran(p2), Ran(p3), Ran(c)); }
    shape *clone() const { return new triangle(p1, p2, p3, c); }

    triangle &operator=(const triangle &t)
    {
        p1 = t.p1;
        p2 = t.p2;
        p3 = t.p3;
        c = t.c;
        return *this;
    }

    void scale(point2f s)
    {
        p1 *= s;
        p2 *= s;
        p3 *= s;
    }

    friend std::ostream &operator<<(std::ostream &os, const triangle &p);
};
std::ostream &operator<<(std::ostream &os, const triangle &p) { return os << "[(" << p.p1 << ", " << p.p2 << ", " << p.p3 << "), " << p.c << ']'; }