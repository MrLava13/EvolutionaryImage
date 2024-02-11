#pragma once
#include "pool.hpp"
#include <fstream>
#include "shapes/circle.hpp"
#include "shapes/rectangle.hpp"
#include "shapes/triangle.hpp"

class imageSaver
{
public:
    struct segment
    {
        pool pools;
        bounds size;
    };

private:
    segment *segments = nullptr;
    int32_t poolCount = 0;

    void wipe()
    {
        if (segments != nullptr)
            delete[] segments;
        segments = nullptr;
    }

    shape *loadFrom(shapeType t)
    {
        if (t == shapeType::circle)
            return new circle;
        if (t == shapeType::triangle)
            return new triangle;
        if (t == shapeType::rectangle)
            return new rectangle;
        return nullptr;
    }

public:
    imageSaver() {}
    imageSaver(int32_t poolCount_) : poolCount(poolCount_), segments(new segment[poolCount]) {}
    imageSaver(imageSaver &s) = delete;
    ~imageSaver() { wipe(); }

    void write(std::ofstream &out) const
    {
        out.write((const char *)&poolCount, sizeof(poolCount));
        for (int32_t pool = 0; pool < poolCount; pool++)
        {
            int32_t numShapes = segments[pool].pools.countElements();
            out.write((const char *)&numShapes, sizeof(numShapes));
            for (int32_t s = 0; s < numShapes; s++)
            {
                char st = static_cast<char>(segments[pool].pools[s].s->getShapeType());
                out.write((const char *)&st, sizeof(st));
                segments[pool].pools[s].s->write(out);
            }
        }
    }

    void write(std::ifstream &in)
    {
        shape *sha;
        wipe();
        in.read((char *)&poolCount, sizeof(poolCount));
        segments = new segment[poolCount];
        for (int32_t pool = 0; pool < poolCount; pool++)
        {
            int32_t numShapes;
            in.read((char *)&numShapes, sizeof(numShapes));
            for (int32_t s = 0; s < numShapes; s++)
            {
                char st;
                in.read((char *)&st, sizeof(st));
                sha = loadFrom(static_cast<shapeType>(st));
                sha->read(in);
                segments[pool].pools.appendShape(sha);
            }
        }
    }

    segment &operator[](int32_t i) { return segments[i]; }
    const segment &operator[](int32_t i) const { return segments[i]; }
};
