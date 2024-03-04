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
    int32_t startingCapacity = 5000,
            parentCount = 5,
            childrenCount = 50; /*Children per parent*/

    int32_t threadCount = std::thread::hardware_concurrency();

private:
    bounds boundingbox;

    image last, current, gt;
    parentPool *pp1, *pp2;

    float lastPerf = INFINITY;

    static void runThread(evolution *e, int32_t start, int32_t end)
    {
        /*
        image current;
        for (auto i = e->data.begin() + start; i != e->data.begin() + end; ++i)
        {
            current = e->last;
            (*i)->s->addShape(&current);
            (*i)->val = e->gt.compareImages(current);
            if ((*i)->val == e->lastPerf)
                (*i)->val = INFINITY;
            e->pp1.tryAdd(*i);
        } */
    }

public:
    evolution() : pp1(new parentPool(parentCount)), pp2(new parentPool(parentCount)) {}
~evolution(){
    delete pp1;
    delete pp2;
}

    bool addBestToImage()
    {
        if (lastPerf > (*pp1)[0].val)
        {
            lastPerf = (*pp1)[0].val;
            current = last;
            (*pp1)[0].s->addShape(&current);
            last = current;
            return true;
        }
        return false;
    }
    void clear()
    {
        pp1->reset();
        pp2->reset();
        pp1->wipe(); // fixes the memory leak until I find what dumb thing is broken
        pp2->wipe();
    }

    void runThreadedStep()
    {
        if (threadCount <= 1)
        {
            return runStep();
        }

        std::vector<std::thread> threads(threadCount);

        int32_t size = 4, // data.getSize() / threadCount,
            offset = 4;   // data.getSize() % threadCount;
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
        pool::poolVal tmpVal;
        // Test each
        if (pp2->getSize())
        {
            int32_t s = parentCount * childrenCount;
            for (int32_t i = 0; i < s; i++)
            {
                tmpVal.replaceShape((*pp2)[rando::randMaxInt(pp2->getSize())].s->regenerate((*pp2)[rando::randMaxInt(pp2->getSize())].s->getColor()));

                current = last;
                tmpVal.s->addShape(&current);
                tmpVal.val = gt.compareImages(current);
                if (tmpVal.val == lastPerf)
                    tmpVal.val = INFINITY;
                pp1->tryAdd(&tmpVal);
            }
            return;
        }
        for (int32_t i = 0; i < startingCapacity; i++)
        {
            // Generate shape
            tmpVal.replaceShape(getRandomShape());
            tmpVal.s->setRandom(boundingbox, &last);

            current = last;
            tmpVal.s->addShape(&current);
            tmpVal.val = gt.compareImages(current);
            if (tmpVal.val == lastPerf)
                tmpVal.val = INFINITY;
            pp1->tryAdd(&tmpVal);
        }
    }

    void prepNext() { std::swap(pp1, pp2); }

    float getBest() const { return (*pp1)[0].val; }

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
