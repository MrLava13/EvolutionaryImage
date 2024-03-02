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
private:
    int32_t width = 0, height = 0, total = 0;

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
    image(point2 s) : image(s.x, s.y) {}
    image(int32_t width_, int32_t height_)
        : width(width_), height(height_), total(width_ * height_),
          imageBounds(0, width_ - 1, 0, height_ - 1), pixels(new colorInt[total]) {}

    // Copy the given image
    image(const image &i) : image(i.width, i.height) { std::copy(i.pixels, i.pixels + i.total, pixels); }
    image(image &&i) noexcept : width(std::exchange(i.width, 0)), height(std::exchange(i.height, 0)), total(std::exchange(i.total, 0))
    {
        std::swap(imageBounds, i.imageBounds);
        std::swap(pixels, i.pixels);
    }

    image &operator=(const image &i) noexcept
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
        std::copy(i.pixels, i.pixels + i.total, pixels);
        total = i.total;
        width = i.width;
        height = i.height;
        imageBounds = i.imageBounds;

        return *this;
    }
    image &operator=(image &&i) noexcept
    {
        if (this != &i)
        {
            width = std::exchange(i.width, 0);
            height = std::exchange(i.height, 0);
            total = std::exchange(i.total, 0);
            std::swap(imageBounds, i.imageBounds);
            std::swap(pixels, i.pixels);
        }
        return *this;
    }

    ~image()
    {
        if (pixels != nullptr)
        {
            delete[] pixels;
            pixels = nullptr;
        }
    }

    const int32_t &getWidth() const { return width; }
    const int32_t &getHeight() const { return height; }
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
    const color getPixel(const point2 &p) const { return getPixel(p.x, p.y); }
    /**
     * @brief Gets the pixel on the screen, even if it is off
     * @param p
     * @return
     */
    const color getClampedPixel(const point2 &p) const { return getPixel(p.clampX(0, width - 1).x, p.clampY(0, height - 1).y); }

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
        int64_t r = 0, g = 0, b = 0, d = maxX * maxY;

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
                r += t.r;
                g += t.g;
                b += t.b;
            }
        }
        return color((int8_t)(r / d), (int8_t)(g / d), (int8_t)(b / d));
    }

    void drawBounds(const bounds &b, const color &c)
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
        assert(width == im.width);
        assert(height == im.height);

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
            std::swap(x0, x1);
        if (y >= height || y < 0 || x1 < 0 || x0 > width)
            return;

        std::fill(pixels + getPixelPos(x0 > 0 ? x0 : 0, y), pixels + getPixelPos(x1 < width ? x1 : width, y), c);
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

    image cut(const bounds &area) const
    {
        assert(area.isIn(imageBounds.min));
        assert(area.isIn(imageBounds.max));

        image output(area.max.x - area.min.x, area.max.y - area.min.y);
        colorInt *pix = output.pixels;

        for (int32_t y = area.min.y; y < area.max.y; y++)
        {
            for (int32_t x = area.min.x; x < area.max.x; x++)
            {
                *(pix++) = pixels[getPixelPos(x, y)];
            }
        }

        return output;
    }

    void paste(const point2 &point, const image &im)
    {
        assert(point.x >= 0);
        assert(point.y >= 0);
        assert(point.x + im.width < width);
        assert(point.y + im.height < height);

        colorInt *pix = im.pixels;

        for (int32_t y = point.y; y < point.y + im.height; y++)
        {
            for (int32_t x = point.x; x < point.x + im.width; x++)
            {
                setPixel(x, y, (pix++)->c);
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