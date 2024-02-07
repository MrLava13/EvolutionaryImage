#pragma once
#include "image.hpp"
#include "shapes/triangle.hpp"
#include <algorithm>
#include <thread>
#include "randomShape.hpp"

class fullEvolution
{
public:
    struct lData
    {
        shape *s = nullptr;
        float val = INFINITY;

        lData() : s(getRandomShape()) {}
        //~lData() { clearOld(); } // me too dum to implement true destructor without segfault

        void clearOld()
        {
            if (s != nullptr)
            {
                delete s;
                s = nullptr;
            }
        }

        void getNewShape()
        {
            clearOld();
            s = getRandomShape();
            val = INFINITY;
        }

        lData &operator=(const lData &d)
        {
            // clearOld();
            s = d.s;
            val = d.val;
            return *this;
        }
        lData &operator=(shape *ns)
        {
            clearOld();
            s = ns;
            val = INFINITY;
            return *this;
        }
    };

    int32_t startingCapacity = 1000, normCap = 200, childrenCount = 20, threadCount = std::thread::hardware_concurrency();

    bounds boundingbox;

private:
    image last, current, gt;
    std::vector<lData> pool;

    image::cachedRows cacheH;

    float lastPerf;

    static void runThread(fullEvolution *e, int32_t start, int32_t end)
    {
        image current;
        for (auto i = e->pool.begin() + start; i != e->pool.begin() + end; ++i)
        {
            current = e->last;
            i->s->addShape(&current);
            i->val = e->gt.compareImages(current);
            /*
            i->val = e->gt.compareBounds(
                current,
                //e->gt.getBounds(),
                 i->s->getBounds(),
                e->cacheH);  //*/
            if (i->val == e->lastPerf)
                i->val = INFINITY;
        }
    }

public:
    fullEvolution() {}

    bool addBestToImage()
    {
        current = last;
        pool[0].s->addShape(&current);
        if (lastPerf > gt.compareImages(current))
        // if (1)
        {
            last = current;
            return true;
        }
        return false;
    }
    void clearAndFill()
    {
        pool = std::vector<lData>(startingCapacity);
        for (lData &t : pool)
        {
            t.s->setRandom(boundingbox, &last);
        }

        /*

        for (auto i = pool.begin() + (pool.size() / pruneAmount); i != pool.end(); ++i)
        {
            i->getNewShape();
            i->s->setRandom(boundingbox, &last);
        }
        */
    }

    void runThreadedStep()
    {
        if (threadCount <= 1)
        {
            return runStep();
        }

        lastPerf = gt.compareImages(last);
        cacheH = gt.getCachedH(last);

        std::vector<std::thread> threads(threadCount);

        int32_t size = pool.size() / threadCount, offset = pool.size() % threadCount;
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
        float noChange = gt.compareImages(last);
        cacheH = gt.getCachedH(last);
        // Test each
        for (lData &t : pool)
        {
            current = last;
            t.s->addShape(&current);
            // t.val = gt.compareImage(current);
            // t.val = gt.compareBounds(current, t.s->getBounds(), cacheH);
            if (t.val == noChange)
            {
                t.val = INFINITY;
            }
        }
    }

    void sort()
    {
        // Sort from lowest to highest
        std::sort(pool.begin(), pool.end(), [](lData &a, lData &b)
                  { return a.val < b.val; });

        /*
        // Copy the ones that won to the new array
        std::vector<lData> arr(pool.size() / pruneAmount);
        std::copy(pool.begin(), pool.begin() + arr.size(), arr.begin());
        pool = arr; */
    }

    void resize() { pool.resize(normCap); }

    void refill()
    {
        int counter = pool.size() / childrenCount;
        auto end = pool.begin() + counter;
        for (auto it = pool.begin(); it != end; ++it)
        {
            for (int i = 1; i < childrenCount; i++)
            {
                pool[counter] = it->s->genFromSelf();
                counter++;
            }
        }

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

    float getBest() const { return pool[0].val; }

    image &getOutImage() { return last; }
    void setGTImage(const image &im)
    {
        gt = im;
        last = current = image(im.getWidth(), im.getHeight());
    }

    void setBound(bounds b) { boundingbox = b; }
};
