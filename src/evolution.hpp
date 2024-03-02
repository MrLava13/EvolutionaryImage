#pragma once
#include "image.hpp"
#include "shapes/triangle.hpp"
#include "pool.hpp"
#include <algorithm>
#include <thread>
#include "randomShape.hpp"

class evolution
{
public:
    class parentPool : public pool
    {
    protected:
        void popShapeInto(poolVal *pv, int32_t spot)
        {
            assert(spot < maxSize);
            assert(spot >= 0);

            if (shapes[maxSize - 1] != nullptr)
                delete shapes[maxSize - 1];

            for (int32_t i = maxSize - 1; i > spot; i--)
            {
                shapes[i] = shapes[i - 1];
            }

            shapes[spot] = pv;
        }

    public:
        float min = INFINITY;

        parentPool(int32_t s) : pool(s) {}

        void reset()
        {
            min = INFINITY;
            resetAll();
        }

        void tryAdd(poolVal *p)
        {
            if (size < maxSize)
            {
                shapes[size++] = new poolVal(std::exchange(p->s, nullptr), p->val);
                min = min > p->val ? p->val : min;
                return;
            }
            if (p->val > min)
                return;

            min = p->val;

            for (int32_t i = 0; i < size; i++)
            {
                if (shapes[i]->val > p->val)
                {
                    if (i == size)
                        min = p->val;
                    if (size < maxSize)
                        size++;

                    popShapeInto(new poolVal(std::exchange(p->s, nullptr), p->val), i);
                    break;
                }
            }
        }
    };

public:
    int32_t startingCapacity = 500,
            parentCount = 5,
            childrenCount = 50; /*Children per parent*/

    int32_t threadCount = std::thread::hardware_concurrency();

    bounds boundingbox;

    image last, current, gt;
    pool data;
    parentPool pp;

private:
    float lastPerf = INFINITY;

    static void runThread(evolution *e, int32_t start, int32_t end)
    {
        image current;
        for (auto i = e->data.begin() + start; i != e->data.begin() + end; ++i)
        {
            current = e->last;
            (*i)->s->addShape(&current);
            (*i)->val = e->gt.compareImages(current);
            if ((*i)->val == e->lastPerf)
                (*i)->val = INFINITY;
            e->pp.tryAdd(*i);
        }
    }

public:
    evolution() : data(startingCapacity), pp(parentCount) {}

    bool addBestToImage()
    {
        if (lastPerf > pp[0].val)
        {
            lastPerf = pp[0].val;
            current = last;
            pp[0].s->addShape(&current);
            last = current;
            return true;
        }
        return false;
    }
    void clearAndFill()
    {
        pp.reset();
        pp.wipe();  //fixes the memory leak until I find what dumb thing is broken
        data.resetAll();

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
        // Test each
        for (auto &t : data)
        {
            current = last;
            t->s->addShape(&current);
            t->val = gt.compareImages(current);
            if (t->val == lastPerf)
                t->val = INFINITY;
            pp.tryAdd(t);
        }
    }

    void resize()
    {
        data.resize(parentCount * childrenCount);
    }

    void refill()
    {
        data.resetAll();
        for (int p = 0; p < pp.getSize(); p++)
        {
            for (int c = 0; c < childrenCount; c++)
            {
                data.appendShape(pp[p].s->regenerate(pp[rando::randMaxInt(pp.getSize())].s->getColor()));
            }
        }
    }

    float getBest() const { return pp[0].val; }

    image &getOutImage() { return last; }
    image &getGTImage() { return gt; }
    void setGTImage(const image &im)
    {
        gt = im;
        last = current = image(im.getWidth(), im.getHeight());
        boundingbox = im.getBounds();
    }

    void setBound(bounds b) { boundingbox = b; }
};
