#pragma once
#include "../shape.hpp"

class rectangle : public shape
{
private:
    float angle;
    point2f center, size;
    color c;

public:
    rectangle(float angle_, point2f center_, point2f size_, color c_)
        : angle(angle_), center(center_), size(size_), c(c_) {}
    rectangle() {}

    void setRandom(const bounds &b, [[maybe_unused]] const image *im)
    {
        // bounds tmp;
        angle = rando::randMinMax(-3.14159, 3.14159);
        size = {rando::randMinMax(3.0f, b.max.x), rando::randMinMax(3.0f, b.max.y)};
        center = {rando::randMinMax(size.x * 0.5f + b.min.x, b.max.x - size.x * 0.5f), rando::randMinMax(size.y * 0.5f + b.min.y, b.max.y - size.y * 0.5f)};
        //    tmp = getBounds();
        // c = im->getClampedPixel(center);
        //  c = im->getColorAv<7>(center);
        // c = im->getPixel(rando::randMinMax(0,im->getWidth() - 1),rando::randMinMax(0,im->getHeight() - 1));
        c = rando::getRandColor();
        // c = rando::randBool() ? rando::getRandColor() : im->getClampedPixel(center);
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

    const color &getColor() const { return c; }
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

    void write(std::ofstream &out) const
    {
        out.write((const char *)&angle, sizeof(angle));
        center.write(out);
        size.write(out);
        c.write(out);
    }
    void read(std::ifstream &in)
    {
        in.read((char *)&angle, sizeof(angle));
        center.read(in);
        size.read(in);
        c.read(in);
    }

    shapeType getShapeType() const { return shapeType::rectangle; }

    shape *genFromSelfAndColor(const color &col) const { return new rectangle(rando::rand(angle), rando::rand(center), rando::rand(size), rando::rand(rando::randBool() ? c : col)); }
    shape *genFromSelf() const { return new rectangle(rando::rand(angle), rando::rand(center), rando::rand(size), rando::rand(c)); }
    shape *genFromParent(const shape *s) const
    {
        // I think I am commiting a crime with this casting
        const rectangle *r = (const rectangle *)s;
        return new rectangle(rando::rand(angle, r->angle), rando::rand(center, r->center), rando::rand(size, r->size), rando::rand(c, r->c));
    }

    shape *clone() const { return new rectangle(angle, center, size, c); }

    void scale(point2f s) { size *= s; }
};
