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
    circle(int32_t radius, point2 center, color c)
        : radius(radius), center(center), c(c) {}

    void setRandom(bounds b, [[maybe_unused]] const image *im)
    {
        radius = Ran.randMinMax(3, (int32_t)(b.max.x / 4));
        center = {
            Ran.randMinMax(radius / 2 + (int32_t)b.min.x, (int32_t)b.max.x - radius / 2),
            Ran.randMinMax(radius / 2 + (int32_t)b.min.y, (int32_t)b.max.y - radius / 2)};
        // c = im->getColorAv<3>(center);
        c = Ran.getRandColor();
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


    bounds getBounds() const { return bounds(center.x - radius, center.x + radius, center.y - radius, center.y + radius); }

    shape *genFromSelf() const { return new circle(Ran(radius), Ran(center), Ran(c)); }
    shape *clone() const { return new circle(radius, center, c); }

    void scale(point2f s) { radius *= abs(1 - s.x) > abs(1 - s.y) ? s.x : s.y; }

    friend std::ostream &operator<<(std::ostream &os, const circle &p);
};
std::ostream &operator<<(std::ostream &os, const circle &p)
{
    return os << "[(" << p.radius << ", " << p.center << "), " << p.c << ']';
}