#pragma once
#include <ostream>
#include <fstream>
#include "shape.hpp"

class pool
{
public:
    struct poolVal
    {
        shape *s = nullptr;
        float val = INFINITY;

        poolVal() {}
        poolVal(shape *s_, float val_) : s(s_), val(val_) {}
        poolVal(poolVal &&p) : val(std::exchange(p.val, 0))
        {
            std::swap(s, p.s);
        }
        ~poolVal()
        {
            if (s != nullptr)
            {
                delete s;
                s = nullptr;
            }
        }

        void replaceShape(shape *sha)
        {
            if (s != nullptr)
                delete s;
            s = sha;
        }
    };

protected:
    int32_t maxSize, size = 0;
    poolVal **shapes;

    void trimFrom(poolVal **start)
    {
        // Delete all the nodes after the given node
        for (poolVal **s = start; s != shapes + maxSize; s++)
        {
            if (!*s)
            {
                break;
            }
            delete *s;
            *s = nullptr;
        }
    }

    void fill() { std::fill_n(shapes, maxSize, nullptr); }

public:
    pool() : pool(0) {}
    pool(int32_t s) : maxSize(s), shapes(new poolVal *[s])
    {
        assert(s > 0);
        fill();
    }

    pool(const pool &p) = delete;      // No copy
    pool &operator=(pool &p) = delete; // No copy

    pool(pool &&p) noexcept
    {
        std::swap(maxSize, p.maxSize);
        std::swap(size, p.size);
        std::swap(shapes, p.shapes);
    }
    pool &operator=(pool &&p) noexcept
    {
        if (this != &p)
        {
            std::swap(maxSize, p.maxSize);
            std::swap(size, p.size);
            std::swap(shapes, p.shapes);
        }
        return *this;
    }

    ~pool()
    {
        for (int32_t i = 0; i < maxSize; i++)
        {
            if (shapes[i])
                delete shapes[i];
        }
        delete[] shapes;
        shapes = nullptr;
    }

    void resize(int32_t s)
    {
        assert(s > 0);
        if (s == maxSize)
            return;

        if (s > maxSize)
        {
            poolVal **nShapes = new poolVal *[s];
            std::copy(shapes, shapes + maxSize, nShapes);
            std::fill(nShapes + maxSize, nShapes + s, nullptr);

            // Transfer vals
            delete[] shapes;
            shapes = nShapes;
            maxSize = s;
            size = size > maxSize ? maxSize : size;
            return;
        }

        size = size > s ? s : size;
    }

    void remaxsize(int32_t s)
    {
        assert(s > 0);
        if (s == maxSize)
            return;

        poolVal **nShapes = new poolVal *[s];

        if (s > maxSize)
        {
            std::copy(shapes, shapes + maxSize, nShapes);
            std::fill(nShapes + maxSize, nShapes + s, nullptr);
        }
        else
        {
            trimFrom(shapes + s);
            std::copy(shapes, shapes + s, nShapes);
        }

        // Transfer vals
        delete[] shapes;
        shapes = nShapes;
        maxSize = s;
        size = size > maxSize ? maxSize : size;
    }

    void sliceAndRefill(int32_t sliceAt)
    {
        assert(sliceAt < size);
        trimFrom(shapes + sliceAt);
        size = sliceAt;
    }

    void resetAll()
    {
        size = 0;
    }

    void replaceAt(int32_t i, shape *s)
    {
        assert(i < maxSize);
        if (shapes[i] != nullptr)
            delete shapes[i]->s;
        shapes[i]->s = s;
    }

    void appendShape(shape *shape)
    {
        assert(size < maxSize);
        if (shapes[size])
            return replaceAt(size++, shape);
        (shapes[size++] = new poolVal)->s = shape;
    }

    int32_t countElements() const
    {
        int32_t output = 0;
        for (; output < maxSize && shapes[output]; output++)
            ;
        return output;
    }

    void wipe() { trimFrom(shapes); }

    bool exists(int32_t i) const { return i < 0 || i > size || shapes[i]; }

    int32_t getSize() const { return size; }
    void setSize(int32_t s) { size = s; }

    poolVal **begin() { return shapes; }
    poolVal **end() { return shapes + size; }

    const poolVal &operator[](int32_t i) const
    {
        assert(i < size);
        return *shapes[i];
    }
    poolVal &operator[](int32_t i)
    {
        assert(i < size);
        return *shapes[i];
    }

    friend std::ostream &operator<<(std::ostream &o, const pool &p);
};

std::ostream &operator<<(std::ostream &o, const pool &p)
{
    if (p.getSize() == 0)
    {
        o << "pool is labeled empty\n";
    }
    for (pool::poolVal **i = p.shapes; i != p.shapes + p.maxSize; i++)
    {
        o << i << ": ";
        if ((*i) == nullptr)
        {
            o << " NULL/noexist";
        }
        else
        {
            if ((*i)->s == nullptr)
            {
                o << "NULLSHAPE";
            }
            else
            {
                // print shape type
                if ((*i)->s->getShapeType() == shapeType::circle)
                    o << "Circle";
                else if ((*i)->s->getShapeType() == shapeType::triangle)
                    o << "Triangle";
                else if ((*i)->s->getShapeType() == shapeType::rectangle)
                    o << "Rectangle";
                else
                    o << "Unknown Shape";

                o << '-' << std::hex << (*i);
            }
            o << '-' << (*i)->val;
        }

        o << "\n";
    }

    return o;
}