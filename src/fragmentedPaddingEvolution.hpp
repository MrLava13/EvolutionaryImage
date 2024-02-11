#pragma once
#include "image.hpp"
#include "evolution.hpp"

class fragmentedPaddingEvolution
{
public:
    int32_t fragmentationSize = 96,
            paddingSize = 4;

    int32_t threadCount = std::thread::hardware_concurrency();

    struct threadManager
    {
        struct threaddedStatus
        {
            bool ir = false;
            int32_t it, cc;
            float q;

            threaddedStatus() {}
            ~threaddedStatus() {}

            bool isRunning() const { return ir; }
            int32_t getIter() const { return it; }
            float getQuality() const { return q; }
            int32_t getCurrent() const { return cc; }
        };

        int32_t threadCount;
        threaddedStatus *threads;

        std::vector<std::thread> ts;

        threadManager(int32_t count) : threadCount(count), threads(new threaddedStatus[count]), ts(count) {}
        ~threadManager() { delete[] threads; }

        bool isRunning()
        {
            for (int32_t i = 0; i < threadCount; i++)
            {
                if (threads[i].isRunning())
                {
                    return true;
                }
            }
            return false;
        }
    };

private:
    image gt;
    evolution *frags;
    bounds *bs;

    int32_t fragCount;

    int32_t current = 0;

    static void runThread(evolution *e)
    {
        e->runStep();
    }

    static void runAll(fragmentedPaddingEvolution *e, threadManager::threaddedStatus *s, int32_t maxIter, int32_t generations, float maxQ)
    {
        evolution *ev;
        s->ir = true;
        while ((ev = e->askForWork()) != nullptr)
        {
            s->cc = ev - e->frags;
            s->it = 0;
            float best = INFINITY;
            for (int b = 0; b < maxIter && best > maxQ; b++)
            {
                ev->clearAndFill();
                for (int i = 0; i < generations; i++)
                {
                    ev->runStep();
                    ev->sort();
                    best = ev->getBest();
                    if (i == 0)
                        ev->resize();
                    ev->refill();
                }
                if (!ev->addBestToImage())
                {
                    b--;
                    continue;
                }
                s->it = b;
                s->q = best;
            }
        }
        s->ir = false;
    }

    evolution *askForWork()
    {
        if (hasMore())
            return frags + (current++);

        return nullptr;
    }

public:
    fragmentedPaddingEvolution(/* args */) {}
    ~fragmentedPaddingEvolution()
    {
        delete[] frags;
        delete[] bs;
    }

    void setGTImage(const image &im) { gt = im; }

    void fragment()
    {
        // missing edge case for if smaller than both... the menace

        int32_t xCount = gt.getWidth() / fragmentationSize,
                xPad = gt.getWidth() % fragmentationSize,
                yCount = gt.getHeight() / fragmentationSize,
                yPad = gt.getHeight() % fragmentationSize;

        fragCount = (xCount * yCount) +
                    (xPad ? yCount : 0) +
                    (yPad ? xCount : 0) +
                    (xPad && yPad ? 1 : 0);

        evolution *currentEv = frags = new evolution[fragCount];
        bounds *currentBS = bs = new bounds[fragCount];

        image temp;
        bounds tb;

        for (int32_t y = 0; y < yCount; y++)
        {
            for (int32_t x = 0; x < xCount; x++)
            {
                *currentBS = bounds(
                                 x * fragmentationSize, (x + 1) * fragmentationSize,
                                 y * fragmentationSize, (y + 1) * fragmentationSize)
                                 .clamp(gt.getBounds());
                tb = (currentBS++)->expand(paddingSize).clamp(gt.getBounds());
                temp = gt.cut(tb);
                (currentEv++)->setGTImage(temp);
            }
            if (xPad)
            {
                *currentBS = bounds(
                                 gt.getWidth() - xPad, gt.getWidth(),
                                 y * fragmentationSize, (y + 1) * fragmentationSize)
                                 .clamp(gt.getBounds());
                tb = (currentBS++)->expand(paddingSize).clamp(gt.getBounds());
                temp = gt.cut(tb);
                (currentEv++)->setGTImage(temp);
            }
        }

        if (yPad)
        {
            for (int32_t x = 0; x < xCount; x++)
            {
                *currentBS = bounds(
                                 x * fragmentationSize, (x + 1) * fragmentationSize,
                                 gt.getHeight() - yPad, gt.getHeight())
                                 .clamp(gt.getBounds());
                tb = (currentBS++)->expand(paddingSize).clamp(gt.getBounds());
                temp = gt.cut(tb);
                (currentEv++)->setGTImage(temp);
            }
        }

        if (xPad && yPad)
        {
            *currentBS = bounds(
                             gt.getWidth() - xPad, gt.getWidth(),
                             gt.getHeight() - yPad, gt.getHeight())
                             .clamp(gt.getBounds());
            tb = (currentBS++)->expand(paddingSize).clamp(gt.getBounds());
            temp = gt.cut(tb);
            (currentEv++)->setGTImage(temp);
        }
    }

    void clearAndFill() { frags[current].clearAndFill(); }
    bool addBestToImage() { return frags[current].addBestToImage(); }

    void runStep() { frags[current].runThreadedStep(); }

    void sort() { frags[current].sort(); }

    float getBest() { return frags[current].getBest(); }
    void resize() { frags[current].resize(); }
    void refill() { frags[current].refill(); }

    bool hasMore() { return current < fragCount; }
    void nextFrag() { current++; }

    threadManager *runFor(int32_t maxIter, int32_t generations, float maxQ)
    {
        assert(threadCount > 0);

        threadManager *output = new threadManager(threadCount);

        threadManager::threaddedStatus *c = output->threads;
        for (std::thread &t : output->ts)
        {
            t = std::thread(runAll, this, c++, maxIter, generations, maxQ);
            t.detach();
        }

        return output;
    }

    void runThreadedStep()
    {
        if (threadCount <= 1)
        {
            return runStep();
        }

        evolution *currentEv = frags;

        std::vector<std::thread> threads(threadCount > fragCount ? threadCount : fragCount);
        for (std::thread &t : threads)
        {
            t = std::thread(runThread, currentEv++);
        }

        for (std::thread &t : threads)
        {
            t.join();
        }
    }

    image deFragment() const
    {
        image output(gt.getWidth(), gt.getHeight()), tmp;

        evolution *currentEv = frags;
        bounds *currentBS = bs, bt;

        for (int32_t i = 0; i < fragCount; i++)
        {
            float xoffset = !currentBS->min.x ? 0 : paddingSize;
            float yoffset = !currentBS->min.y ? 0 : paddingSize;
            bt = {
                xoffset,
                currentBS->max.x - currentBS->min.x + xoffset,
                yoffset,
                currentBS->max.y - currentBS->min.y + yoffset};
            tmp = (currentEv++)->getOutImage().cut(bt);
            point2 p(currentBS->min.x, currentBS->min.y);
            output.paste(p, tmp);
            currentBS++;
        }

        return output;
    }

    image deFragmentGT() const
    {
        image output(gt.getWidth(), gt.getHeight()), tmp;

        evolution *currentEv = frags;
        bounds *currentBS = bs, bt;

        for (int32_t i = 0; i < fragCount; i++)
        {
            float xoffset = !currentBS->min.x ? 0 : paddingSize,
                  yoffset = !currentBS->min.y ? 0 : paddingSize;
            bt = {
                xoffset,
                currentBS->max.x - currentBS->min.x + xoffset,
                yoffset,
                currentBS->max.y - currentBS->min.y + yoffset};
            tmp = (currentEv++)->getGTImage().cut(bt);
            point2 p(currentBS->min.x, currentBS->min.y);
            output.paste(p, tmp);
            currentBS++;
        }

        return output;
    }
};