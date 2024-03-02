#pragma once

#include "../shape.hpp"

class circle : public shape
{
private:
    int32_t radius = 25;
    point2 center = {100, 100};
    color c = {255, 0, 0};

public:
    circle() {}
    circle(int32_t radius_, point2 center_, color c_)
        : radius(radius_), center(center_), c(c_) {}

    void setRandom(const bounds &b, [[maybe_unused]] const image *im)
    {
        radius = rando::randMinMax(3, (int32_t)(b.max.x / 4));
        center = {
            rando::randMinMax(radius / 2 + (int32_t)b.min.x, (int32_t)b.max.x - radius / 2),
            rando::randMinMax(radius / 2 + (int32_t)b.min.y, (int32_t)b.max.y - radius / 2)};
        // c = im->getColorAv<3>(center);
        c = rando::getRandColor();
    }

    void addShape(image *v) const
    {
        int x = 0;
        int y = radius;
        int m = 5 - 4 * radius;

        while (x <= y)
        {
            for (int xx = center.x - y; xx <= center.x + y; xx++)
            {
                v->setPixel(xx, center.y - x, c);
                v->setPixel(xx, center.y + x, c);
            }

            if (m > 0)
            {
                for (int xx = center.x - x; xx <= center.x + x; xx++)
                {
                    v->setPixel(xx, center.y - y, c);
                    v->setPixel(xx, center.y + y, c);
                }

                y--;
                m -= 8 * y;
            }

            x++;
            m += 8 * x + 4;
        }
    }

    void write(std::ofstream &out) const {}
    void read(std::ifstream &in) {}

    shapeType getShapeType() const { return shapeType::circle; }

    const color &getColor() const { return c; }
    bounds getBounds() const { return bounds(center.x - radius, center.x + radius, center.y - radius, center.y + radius); }

    shape *regenerate(const color &col) const { return new circle(rando::rand(radius), rando::rand(center), rando::rand(c, col)); }
    shape *regenerate() const { return new circle(rando::rand(radius), rando::rand(center), rando::rand(c)); }
    shape *regenerate(const shape *s) const
    {
        // I think I am commiting a crime with this casting
        const circle *r = (const circle *)s;
        return new circle(rando::rand(radius, r->radius), rando::rand(center, r->center), rando::rand(c, r->c));
    }

    shape *clone() const { return new circle(radius, center, c); }

    void scale(point2f s) { radius *= abs(1 - s.x) > abs(1 - s.y) ? s.x : s.y; }

    friend std::ostream &operator<<(std::ostream &os, const circle &p);
};
std::ostream &operator<<(std::ostream &os, const circle &p)
{
    return os << "[(" << p.radius << ", " << p.center << "), " << p.c << ']';
}