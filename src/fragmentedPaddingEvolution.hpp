#pragma once
#include "image.hpp"
#include "evolution.hpp"
#include "fragmentedEvolution.hpp"

class fragmentedPaddingEvolution : public fragmentedEvolution
{
public:
    int32_t paddingSize = 4;

protected:
    bounds *bs = nullptr;

public:
    fragmentedPaddingEvolution(/* args */) {}
    ~fragmentedPaddingEvolution()
    {
        if (bs != nullptr)
            delete[] bs;
    }

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