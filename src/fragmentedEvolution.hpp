#pragma once
#include "image.hpp"
#include "evolution.hpp"

class fragmentedEvolution
{
public:
    int32_t fragmentationSize = 96;

    int32_t threadCount = std::thread::hardware_concurrency();

    struct threadManager
    {
        enum class status
        {
            NOT_STARTED,
            RUNNING,
            FINISHED
        };
        struct threadStatus
        {
            status s = status::NOT_STARTED;
            int32_t it = 0, cc = 0;
            float q = INFINITY;

            threadStatus() {}
            ~threadStatus() {}

            status getStatus() const { return s; }
            int32_t getIter() const { return it; }
            float getQuality() const { return q; }
            int32_t getCurrent() const { return cc; }
        };

        int32_t threadCount = 0;
        threadStatus *threads = nullptr;

        std::vector<std::thread> ts;

        threadManager(int32_t count) : threadCount(count), threads(count <= 0 ? nullptr : new threadStatus[count]), ts(count) {}
        ~threadManager()
        {
            if (threads != nullptr)
                delete[] threads;
        }
        threadManager(threadManager &&t) noexcept
        {
            std::swap(threadCount, t.threadCount);
            std::swap(threads, t.threads);
            std::swap(ts, t.ts);
        }
        threadManager &operator=(threadManager &&t) noexcept
        {
            if (this != &t)
            {
                std::swap(threadCount, t.threadCount);
                std::swap(threads, t.threads);
                std::swap(ts, t.ts);
            }
            return *this;
        }
        threadManager(threadManager &t) = delete;
        threadManager &operator=(threadManager &t) = delete;

        bool isRunning() const
        {
            for (int32_t i = 0; i < threadCount; i++)
            {
                if (threads[i].getStatus() == threadManager::status::RUNNING)
                {
                    return true;
                }
            }
            return false;
        }

        int32_t getLargestFrag() const
        {
            int32_t largest = 0;
            for (int32_t i = 0; i < threadCount; i++)
            {
                largest = largest > threads[i].cc ? largest : threads[i].cc;
            }
            return largest;
        }

        void waitTillFinished() const
        {
            while (isRunning())
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(5ms);
            }
        }

        const threadStatus operator[](int32_t i) const { return threads[i]; }
    };

protected:
    image gt;
    evolution *frags = nullptr;
    point2 *points = nullptr;

    int32_t fragCount, current = 0;

    static void runThread(evolution *e) { e->runStep(); }

    static void runAll(fragmentedEvolution *e, threadManager::threadStatus *s, int32_t maxIter, int32_t generations, float maxQ)
    {
        evolution *ev;
        s->s = threadManager::status::RUNNING;
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
        s->s = threadManager::status::FINISHED;
    }

    evolution *askForWork()
    {
        if (hasMore())
        {
            return frags + (current++);
        }

        return nullptr;
    }

public:
    fragmentedEvolution(/* args */) {}
    ~fragmentedEvolution()
    {
        if (frags != nullptr)
            delete[] frags;
        if (points != nullptr)
            delete[] points;
    }

    void setGTImage(const image &im) { gt = im; }

    void fragment()
    {
        if (frags != nullptr)
            delete[] frags;
        if (points != nullptr)
            delete[] points;

        int32_t xCount = gt.getWidth() / fragmentationSize,
                xPad = gt.getWidth() % fragmentationSize,
                yCount = gt.getHeight() / fragmentationSize,
                yPad = gt.getHeight() % fragmentationSize;

        fragCount = (xCount * yCount) +
                    (xPad ? yCount : 0) +
                    (yPad ? xCount : 0) +
                    (xPad && yPad ? 1 : 0);

        evolution *currentEv = frags = new evolution[fragCount];
        point2 *currentPoint = points = new point2[fragCount];

        image temp;

        for (int32_t y = 0; y < yCount; y++)
        {
            for (int32_t x = 0; x < xCount; x++)
            {
                bounds tb = bounds(
                                x == 0 ? 0 : (x * fragmentationSize), (x + 1) * fragmentationSize,
                                y == 0 ? 0 : (y * fragmentationSize), (y + 1) * fragmentationSize)
                                .clamp(gt.getBounds());
                temp = gt.cut(tb);
                *(currentPoint++) = point2(tb.min.x, tb.min.y);
                (currentEv)->setGTImage(temp);
                (currentEv)->setBound(temp.getBounds().expand(10));
                currentEv++;
            }
            if (xPad)
            {
                bounds tb = bounds(
                                gt.getWidth() - xPad, gt.getWidth(),
                                y == 0 ? 0 : (y * fragmentationSize), (y + 1) * fragmentationSize)
                                .clamp(gt.getBounds());
                temp = gt.cut(tb);
                *(currentPoint++) = point2(tb.min.x, tb.min.y);
                (currentEv)->setGTImage(temp);
                (currentEv)->setBound(temp.getBounds().expand(10));
                currentEv++;
            }
        }

        if (yPad)
        {
            for (int32_t x = 0; x < xCount; x++)
            {
                bounds tb = bounds(
                                x == 0 ? 0 : (x * fragmentationSize), (x + 1) * fragmentationSize,
                                gt.getHeight() - yPad, gt.getHeight())
                                .clamp(gt.getBounds());
                temp = gt.cut(tb);
                *(currentPoint++) = point2(tb.min.x, tb.min.y);
                (currentEv)->setGTImage(temp);
                (currentEv)->setBound(temp.getBounds().expand(10));
                currentEv++;
            }
        }

        if (xPad && yPad)
        {
            bounds tb = bounds(
                            gt.getWidth() - xPad, gt.getWidth(),
                            gt.getHeight() - yPad, gt.getHeight())
                            .clamp(gt.getBounds());
            temp = gt.cut(tb);
            *(currentPoint++) = point2(tb.min.x, tb.min.y);
            (currentEv)->setGTImage(temp);
            (currentEv)->setBound(temp.getBounds().expand(fragmentationSize / 10));
            currentEv++;
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

    int32_t getFragCount() const { return fragCount; }

    threadManager runFor(int32_t maxIter, int32_t generations, float maxQ)
    {
        if (threadCount <= 1)
        {
            return {0};
        }

        threadManager output = threadManager(threadCount);

        threadManager::threadStatus *c = output.threads;
        for (std::thread &t : output.ts)
        {
            (t = std::thread(runAll, this, c++, maxIter, generations, maxQ)).detach();
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
        image output(gt.getWidth(), gt.getHeight());

        evolution *currentEv = frags;
        point2 *currentPoint = points;

        for (int32_t i = 0; i < fragCount; i++)
        {
            output.paste(*(currentPoint++), (currentEv++)->getOutImage());
        }

        return output;
    }

    image deFragmentGT() const
    {
        image output(gt.getWidth(), gt.getHeight());

        evolution *currentEv = frags;
        point2 *currentPoint = points;

        for (int32_t i = 0; i < fragCount; i++)
        {
            output.paste(*(currentPoint++), (currentEv++)->getGTImage());
        }

        return output;
    }
};