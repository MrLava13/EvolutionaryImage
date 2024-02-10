#pragma once
#include "shape.hpp"

class pool
{
public:
    struct poolVal
    {
        shape *s = nullptr;
        float val = INFINITY;

        poolVal() {}
        ~poolVal()
        {
            if (s != nullptr)
            {
                delete s;
                s = nullptr;
            }
        }
    };

private:
    int32_t maxSize, size = 0;
    poolVal **shapes;

    void wipe()
    {
        assert(shapes != nullptr);
        trimFrom(shapes);
    }

    void trimFrom(poolVal **start)
    {
        // Delete all the nodes after the given node
        for (poolVal **s = start; s != shapes + maxSize; s++)
        {
            if (*s == nullptr)
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

    pool(const pool &p) = delete;
    pool &operator=(pool &p) = delete;
    pool &operator=(pool &&p) = delete;

    ~pool()
    {
        wipe();
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

        trimFrom(shapes + s);
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
        if (size == 0)
        {
            return;
        }
        wipe();
        size = 0;
    }

    void appendShape(shape *shape)
    {
        assert(size < maxSize);
        (shapes[size++] = new poolVal)->s = shape;
    }

    int32_t countElements() const
    {
        int32_t output = 0;
        for (; output < maxSize && shapes[output] != nullptr; output++)
            ;
        return output;
    }

    int32_t getSize() const { return size; }

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
};