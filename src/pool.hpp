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

    void fill()
    {
        for (int32_t i = 0; i < maxSize; i++)
        {
            shapes[i] = nullptr;
        }
    }

public:
    pool(int32_t s) : maxSize(s), shapes(new poolVal *[s])
    {
        assert(s > 0);
        fill();
    }
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

            for (int i = 0; i < maxSize; i++)
            {
                nShapes[i] = shapes[i];
            }
            for (int i = maxSize; i < s; i++)
            {
                nShapes[i] = nullptr;
            }

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
            for (int i = 0; i < maxSize; i++)
            {
                nShapes[i] = shapes[i];
            }
            for (int i = maxSize; i < s; i++)
            {
                nShapes[i] = nullptr;
            }
        }
        else
        {
            trimFrom(shapes + s);
            for (int i = 0; i < s; i++)
            {
                nShapes[i] = shapes[i];
            }
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
        for (int i = sliceAt; i < maxSize; i++)
            shapes[i] = nullptr;
        size = sliceAt;
    }

    void resetAll()
    {
        if (size == 0)
        {
            return;
        }
        wipe();
        fill();
        size = 0;
    }

    void appendShape(shape *shape)
    {
        assert(size < maxSize);
        (shapes[size++] = new poolVal)->s = shape;
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