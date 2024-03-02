#pragma once
#include "../shape.hpp"

class triangle : public shape
{
private:
    point2 p1, p2, p3;
    color c;

public:
    triangle() {}
    triangle(point2 p1_, point2 p2_, point2 p3_, color c_)
        : p1(p1_), p2(p2_), p3(p3_), c(c_) {}
    ~triangle() {}
    void setRandom(const bounds &bound, const image *im)
    {
        p1 = rando::randPoint2(bound);
        p2 = rando::randPoint2(bound);
        p3 = rando::randPoint2(bound);
        c = rando::getRandColor();

        // c = im->getColorAv<3>({(int32_t)((p1.x + p2.x + p3.x) / 3), (int32_t)((p1.y + p2.y + p3.y) / 3)});
        // c = im->getClampedPixel({(int32_t)((p1.x + p2.x + p3.x) / 3), (int32_t)((p1.y + p2.y + p3.y) / 3)});
        //  if (c.a == 0)
        // c = im->getPixel(rando::randMinMax(0, im->getWidth() - 1), rando::randMinMax(0, im->getHeight() - 1));
        // c = rando::randBool() ? rando::getRandColor() : im->getClampedPixel({(int32_t)((p1.x + p2.x + p3.x) / 3), (int32_t)((p1.y + p2.y + p3.y) / 3)});
    }
    void addShape(image *v) const { v->drawTriangle(p1, p2, p3, c); }

    const color &getColor() const { return c; }

    bounds getBounds() const
    {
        return {
            p1.min(p2.min(p3)),
            p1.max(p2.max(p3))};
    }

    shape *regenerate() const { return new triangle(rando::rand(p1), rando::rand(p2), rando::rand(p3), rando::rand(c)); }
    shape *regenerate(const color &col) const { return new triangle(rando::rand(p1), rando::rand(p2), rando::rand(p3), rando::rand(c, col)); }
    shape *regenerate(const shape *s) const
    {
        // I think I am commiting a crime with this casting
        const triangle *t = (const triangle *)s;
        return new triangle(rando::rand(p1, t->p1), rando::rand(p2, t->p2), rando::rand(p3, t->p3), rando::rand(c, t->c));
    }

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

    void write(std::ofstream &out) const
    {
        p1.write(out);
        p2.write(out);
        p3.write(out);
        c.write(out);
    }
    void read(std::ifstream &in)
    {
        p1.read(in);
        p2.read(in);
        p3.read(in);
        c.read(in);
    }

    shapeType getShapeType() const { return shapeType::triangle; }

    friend std::ostream &operator<<(std::ostream &os, const triangle &p);
};
std::ostream &operator<<(std::ostream &os, const triangle &p) { return os << "[(" << p.p1 << ", " << p.p2 << ", " << p.p3 << "), " << p.c << ']'; }