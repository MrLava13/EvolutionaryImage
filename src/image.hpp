#pragma once
#include "color.hpp"
#include <assert.h>
#include <cmath>
#include <cstring>
#include <vector>
#include "point.hpp"
#include <iostream>
#include <iomanip>
#include "bounds.hpp"

class image
{
public:
    typedef std::vector<float> cachedRows;

private:
    int32_t width, height, total;

    bounds imageBounds;

    colorInt *pixels = nullptr;
    void fillBottomFlatTriangle(const point2f &v1, const point2f &v2, const point2f &v3, const color &c)
    {
        float invslope1 = (v2.x - v1.x) / (v2.y - v1.y),
              invslope2 = (v3.x - v1.x) / (v3.y - v1.y),

              curx1 = v1.x,
              curx2 = v1.x;

        for (int scanlineY = v1.y; scanlineY <= v2.y; scanlineY++)
        {
            drawLine(curx1, curx2, scanlineY, c);

            curx1 += invslope1;
            curx2 += invslope2;
        }
    }

    void fillTopFlatTriangle(const point2f &v1, const point2f &v2, const point2f &v3, const color &c)
    {
        float invslope1 = (v3.x - v1.x) / (v3.y - v1.y),
              invslope2 = (v3.x - v2.x) / (v3.y - v2.y),

              curx1 = v3.x,
              curx2 = v3.x;

        for (int scanlineY = v3.y; scanlineY > v1.y; scanlineY--)
        {
            drawLine(curx1, curx2, scanlineY, c);

            curx1 -= invslope1;
            curx2 -= invslope2;
        }
    }

public:
    image() {}
    image(int32_t width, int32_t height)
        : width(width), height(height), total(width * height), imageBounds(0, width - 1, 0, height - 1), pixels(new colorInt[total]) {}
    image(const image &i)
        : width(i.width), height(i.height), total(i.total), imageBounds(i.imageBounds), pixels(new colorInt[total])
    {
        colorInt *p = pixels;
        for (colorInt *o = i.pixels; o < i.pixels + total; ++o)
        {
            *(p++) = *o;
        }
    }
    ~image() { delete[] pixels; }

    int32_t getWidth() const { return width; }
    int32_t getHeight() const { return height; }
    const bounds &getBounds() const { return imageBounds; }

    /**
     * @brief Get the index of a pixel from its x and y
     * @param x
     * @param y
     * @return
     */
    inline int32_t getPixelPos(int32_t x, int32_t y) const { return y * width + x; }

    /**
     * @brief Set the given pixel to the given color
     * @param x
     * @param y
     * @param c
     */
    void setPixel(int32_t x, int32_t y, const color &c)
    {
        if (x < 0 || y < 0 || x >= width || y >= height)
        {
            std::cout << "er: (" << x << ", " << y << ")\n";
            return;
        }
        pixels[y * width + x].c = c;
        // std::cout << "er: (" << x << ", " << y << ")\n";
        // assert(y * width + x < total);
    }
    /**
     * @brief Sets the given pixel to the given color
     * @param i
     * @param c
     &*/
    void setPixel(int32_t i, const color &c)
    {
        if (i < 0 || i > total)
            return;
        pixels[i].c = c;
        // std::cout << "er: (" << x << ", " << y << ")\n";
        // assert(y * width + x < total);
    }

    /**
     * @brief Get the pixel at the given coordinates, no safety checks
     * @param x
     * @param y
     * @return
     */
    const color getPixel(int32_t x, int32_t y) const
    {
        if (x < 0 || y < 0 || x >= width || y >= height)
        {
            std::cout << "er: (" << x << ", " << y << ")\n";
            return {};
        }
        assert(y * width + x < total);
        return pixels[y * width + x].c;
    }
    /**
     * @brief Get the pixel of the given coordinates, no checks
     * @param p
     * @return
     */
    const color getPixel(const point2 &p) const
    {
        /*
        if (p.x < 0 || p.y < 0 || p.x >= width || p.y >= height)
        {
            return getClampedPixel(p);
        }*/
        return getPixel(p.x, p.y);
    }
    /**
     * @brief Gets the pixel on the screen, even if it is off
     * @param p
     * @return
     */
    const color getClampedPixel(const point2 &p) const { return getPixel(p.clampX(0, width).x, p.clampY(0, height - 1).y); }

    /**
     * @brief Get the color average around a specified point
     * @tparam size diameter of the average
     * @param p
     * @return
     */
    template <int32_t size>
    color getColorAv(point2 p) const
    {
        if constexpr (size < 2)
        {
            return getClampedPixel(p);
        }
        int32_t maxX = (p.x + size < width ? p.x + size : width), maxY = (p.y + size < height ? p.y + size : height);
        int64_t R = 0, G = 0, B = 0, d = maxX * maxY;

        if (d == 0)
        {
            return getClampedPixel(p);
        }
        p -= size / 2;
        for (int32_t x = p.x; x < maxX; x++)
        {
            for (int32_t y = p.y; y < maxY; y++)
            {
                color t = getPixel(x, y);
                R += t.R;
                G += t.G;
                B += t.B;
            }
        }
        return color((int8_t)(R / d), (int8_t)(G / d), (int8_t)(B / d));
    }

    cachedRows getCachedH(const image &im) const
    {
        cachedRows output(height);
        for (int32_t i = 0; i < total; i++)
            output[i / width] += pixels[i].c.findColorDifference(im[i]);
        return output;

        /*
        for (int32_t y = 0; y < height; y++)
        {
            int32_t yoff = y * width;
            for (int32_t x = 0; x < width; x++)
            {
                output[y] += pixels[yoff + x].findColorDifference(im[yoff + x]);
            }
        }
        return output; */
    }

    /**
     * @brief Compare a section of a image to another image
     * @param im
     * @param b
     * @return
     */
    float compareBounds(const image &im, const bounds &b, const cachedRows &cachedH) const
    {
        bounds area = b.clamp(imageBounds);
        /*
        if (area == imageBounds)
        {
            return compareImages(im);
        } */
        float dif = 0;

        for (int32_t y = 0; y <= area.min.y; y++)
            dif += cachedH[y];

        //*

        for (int32_t y = area.min.y; y < area.max.y; y++)
        {
            int32_t yoff = y * width;
            for (int32_t x = 0; x < width; x++)
            {
                // dif += getPixel(x, y).findColorDifference(im.getPixel(x,y));
                dif += pixels[yoff + x].c.findColorDifference(im[yoff + x]);
            }
        } //*/

        for (int32_t y = area.max.y; y < height; y++)
            dif += cachedH[y];

        return dif / total;
    }

    void drawBounds(bounds b, const color &c)
    {
        drawLine(b.min.x, b.max.x, b.min.y, c);
        drawLine(b.min.x, b.max.x, b.max.y, c);
        drawVLine(b.min.y, b.max.y, b.min.x, c);
        drawVLine(b.min.y, b.max.y, b.max.x, c);
    }

    /**
     * @brief Compare all of the pixels of two images
     * @param im
     * @return
     */
    float compareImages(const image &im) const
    {
        float dif = 0;
        for (int32_t i = 0; i < total; i++)
        {
            dif += pixels[i].c.findColorDifference(im[i]);
        }

        return dif / total;
    }

    void drawVLine(int32_t y0, int32_t y1, int32_t x, const color &c)
    {
        if (y0 > y1)
        {
            std::swap(y0, y1);
        }
        if (x >= width || x < 0 || y1 < 0 || y0 > height)
        {
            return;
        }
        for (int32_t y = y0 > 0 ? y0 : 0; y < (y1 < height ? y1 : height); y++)
        {
            setPixel(x, y, c);
        }
    }

    /**
     * @brief Draw a horizontal line from x0 to x1
     * @param x0
     * @param x1
     * @param y
     * @param c
     */
    void drawLine(int32_t x0, int32_t x1, int32_t y, const color &c)
    {
        if (x0 > x1)
        {
            std::swap(x0, x1);
        }
        if (y >= height || y < 0 || x1 < 0 || x0 > width)
        {
            return;
        }

        colorInt col = c;
        for (auto p = pixels + getPixelPos(x0 > 0 ? x0 : 0, y); p != pixels + getPixelPos(x1 < width ? x1 : width, y); p++)
        {
            *p = col;
        }
        /*
        int32_t start = getPixelPos(x0 > 0 ? x0 : 0, y),
                end = getPixelPos((x1 < width ? x1 : width), y);
        for (int32_t i = start; i < end; i++)
        {
            pixels[i].c = c;
        } //*/
    }

    /**
     * @brief Draw any line with two points
     * @param p1
     * @param p2
     * @param c
     */
    void bresenhamLine(point2 p1, point2 p2, const color &c)
    {
        int32_t dx = std::abs(p2.x - p1.x),
                sx = p1.x < p2.x ? 1 : -1,
                dy = -std::abs(p1.y - p2.y),
                sy = p1.y < p2.y ? 1 : -1,
                error = dx + dy;

        while (true)
        {
            setPixel(p1.x, p1.y, c);
            if (p1.x == p2.x && p1.y == p2.y)
                break;
            int32_t e2 = 2 * error;
            if (e2 >= dy)
            {
                if (p1.x == p2.x)
                    break;
                error += dy;
                p1.x += sx;
            }
            if (e2 <= dx)
            {
                if (p1.y == p2.y)
                    break;
                error += dx;
                p1.y += sy;
            }
        }
    }

    /**
     * @brief Draw a filled in triangle of the given color
     * @param p1
     * @param p2
     * @param p3
     * @param c
     */
    void drawTriangle(point2 p1, point2 p2, point2 p3, const color &c)
    {
        // Sort the points by their y-coordinates
        if (p1.y > p2.y)
            std::swap(p1, p2);
        if (p1.y > p3.y)
            std::swap(p1, p3);
        if (p2.y > p3.y)
            std::swap(p2, p3);

        if (p2.y == p3.y)
            return fillBottomFlatTriangle(p1, p2, p3, c);
        /* check for trivial case of top-flat triangle */
        if (p1.y == p2.y)
            return fillTopFlatTriangle(p1, p2, p3, c);
        /* general case - split the triangle in a topflat and bottom-flat one */
        const point2f v4 = point2f((int)(p1.x + ((float)(p2.y - p1.y) / (float)(p3.y - p1.y)) * (p3.x - p1.x)), p2.y);
        fillBottomFlatTriangle(p1, p2, v4, c);
        fillTopFlatTriangle(p2, v4, p3, c);
    }

    auto begin() { return pixels; }
    const auto begin() const { return pixels; }
    auto end() { return pixels + total; }
    const auto end() const { return pixels + total; }

    image &operator=(const image &i)
    {
        if (pixels == nullptr)
        {
            pixels = new colorInt[i.total];
        }
        else if (i.total != total)
        {
            delete[] pixels;
            pixels = new colorInt[i.total];
        }
        total = i.total;

        colorInt *p = pixels;
        for (colorInt *o = i.pixels; o < i.pixels + total; ++o)
        {
            *(p++) = *o;
        }
        width = i.width;
        height = i.height;
        imageBounds = i.imageBounds;

        return *this;
    }

    color &operator[](int32_t i)
    {
        assert(i < total);
        return pixels[i].c;
    }
    const color &operator[](int32_t i) const
    {
        assert(i < total);
        return pixels[i].c;
    }
};

std::ostream &operator<<(std::ostream &in, const image &im)
{
    in << "-----------\n";
    for (int32_t y = 0; y < im.getHeight(); y++)
    {
        in << std::setw(ceil(log10(im.getHeight())) + 1) << y << '|';
        for (int32_t x = 0; x < im.getWidth(); x++)
        {
            in << (!(im.getPixel(x, y) == color()) ? '*' : ' ') << ' ';
        }

        in << "|\n";
    }
    return in;
}