#pragma once
#include "image.hpp"
#include "shapes/triangle.hpp"
#include "pool.hpp"
#include <algorithm>
#include <thread>
#include "randomShape.hpp"

class fullEvolution
{
public:
    int32_t startingCapacity = 2000,
            parentCount = 8,
            childrenCount = 100;

    int32_t threadCount = std::thread::hardware_concurrency();

    bounds boundingbox;

    image last, current, gt;
    pool data;

private:
    image::cachedRows cacheH;

    float lastPerf;

    static void runThread(fullEvolution *e, int32_t start, int32_t end)
    {
        image current;
        for (auto i = e->data.begin() + start; i != e->data.begin() + end; ++i)
        {
            current = e->last;
            (*i)->s->addShape(&current);
            (*i)->val = e->gt.compareImages(current);
            /*
            (*i)->val = e->gt.compareBounds(
                current,
                //e->gt.getBounds(),
                (*i)->s->getBounds(),
                e->cacheH);  //*/
            if ((*i)->val == e->lastPerf)
                (*i)->val = INFINITY;
        }
    }

public:
    fullEvolution() : data(startingCapacity) {}

    bool addBestToImage()
    {
        current = last;
        data[0].s->addShape(&current);
        // current.drawBounds(data[0].s->getBounds(), {0, 255, 0}); //ESP
        if (lastPerf > gt.compareImages(current))
        {
            last = current;
            return true;
        }
        return false;
    }
    void clearAndFill()
    {
        data.resetAll();
        data.resize(startingCapacity);

        for (int i = 0; i < startingCapacity; i++)
        {
            shape *tmp = getRandomShape();
            tmp->setRandom(boundingbox, &last);
            data.appendShape(tmp);
        }
    }

    void runThreadedStep()
    {
        if (threadCount <= 1)
        {
            return runStep();
        }

        lastPerf = gt.compareImages(last);
        // cacheH = gt.getCachedH(last);

        std::vector<std::thread> threads(threadCount);

        int32_t size = data.getSize() / threadCount, offset = data.getSize() % threadCount;
        for (int32_t i = 0; i < threadCount; i++)
        {
            threads[i] = std::thread(
                runThread,
                this,
                i * size + (i == 0 ? 0 : offset),
                ((i + 1) * size) + offset);
        }

        for (std::thread &t : threads)
        {
            t.join();
        }
    }

    void runStep()
    {
        lastPerf = gt.compareImages(last);
        // cacheH = gt.getCachedH(last);
        // Test each
        for (auto &t : data)
        {
            current = last;
            t->s->addShape(&current);
            t->val = gt.compareImages(current);
            // t.val = gt.compareBounds(current, t.s->getBounds(), cacheH);
            if (t->val == lastPerf)
                t->val = INFINITY;
        }
    }

    void sort()
    {
        // Sort from lowest to highest
        std::sort(data.begin(), data.end(), [](const pool::poolVal *a, const pool::poolVal *b)
                  { return a->val < b->val; });
    }

    void resize()
    {
        //data.resize(100);
        data.resize(parentCount * (childrenCount + 1)); // Add one to keep room for the parents themselves
    }

    void refill()
    {
        data.sliceAndRefill(parentCount);

        ///*
        for (int p = 0; p < parentCount; p++)
        {
            for (int c = 0; c < childrenCount; c++)
            {
                data.appendShape(data[p].s->genFromSelf());
            }
        } //*/

        /*
        int counter = data.getSize() / childrenCount;
        auto end = data.begin() + counter;
        for (auto it = data.begin(); it != end; ++it)
        {
            for (int i = 0; i < childrenCount; i++)
            {
                data.appendShape((*it)->s->genFromSelf());
            }
        } //*/

        /*

        std::vector<lData> arr(pool.size() * childrenCount);
        for (lData &t : pool)
        {
            for (int i = 0; i < childrenCount; i++)
            {
                arr[counter++].s = t.s->genFromSelf();
            }
        }
        pool = arr; */
    }

    float getBest() const { return data[0].val; }

    image &getOutImage() { return last; }
    void setGTImage(const image &im)
    {
        gt = im;
        last = current = image(im.getWidth(), im.getHeight());
    }

    void setBound(bounds b) { boundingbox = b; }
};
