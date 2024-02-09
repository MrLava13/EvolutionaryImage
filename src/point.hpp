#pragma once
#include <cstdint>
#include <cmath>
#include <iostream>

template <class T>
struct base_point
{
    T x, y;

    static constexpr std::streamsize d_size = sizeof(T);

    base_point() : x(0), y(0) {}
    base_point(const T x, const T y) : x(x), y(y) {}
    base_point(const base_point<T> &p) : x(p.x), y(p.y) {}
    base_point(base_point<T> &&p) noexcept : x(std::move(p.x)), y(std::move(p.y)) {}
    template <class F>
    base_point(const base_point<F> &p) : x((T)p.x), y((T)p.y) {}

    /**
     * @brief Clamps the X value between the given values
     * @param min
     * @param max
     * @return
     */
    base_point<T> clampX(T min, T max) const { return {x < min ? min : (x > max ? max : x), y}; }
    /**
     * @brief Clamps the Y value between the given values
     * @param min
     * @param max
     * @return
     */
    base_point<T> clampY(T min, T max) const { return {x, y < min ? min : (y > max ? max : y)}; }

    base_point<T> clamp(T min, T max) const { return {x < min ? min : (x > max ? max : x), y < min ? min : (y > max ? max : y)}; }

    base_point<T> clamp(const base_point<T> &min, const base_point<T> &max) const
    {
        return {x < min.x ? min.x : (x > max.x ? max.x : x),
                y < min.y ? min.y : (y > max.y ? max.y : y)};
    }

    base_point<T> min(const base_point<T> &p) const { return {x < p.x ? x : p.x, y < p.y ? y : p.y}; }
    base_point<T> max(const base_point<T> &p) const { return {x > p.x ? x : p.x, y > p.y ? y : p.y}; }

    /**
     * @brief Find the distance beteen two points
     * @param p
     * @return
     */
    T distance(const base_point<T> &p) const { return std::sqrt((x - p.x) * (x - p.x) + (y + p.y) * (y + p.y)); }

    /**
     * @brief Rotate the original point around the center point
     * @param center
     * @param angle
     * @return
     */
    inline base_point<T> rotate(const base_point<T> &center, T angle) const
    {
        base_point<T> p(*this);
        // Translate the point to the origin
        p -= center;

        // Rotate the point by the angle
        const T c = cos(angle), s = sin(angle);
        base_point<T> output = {p.x * c - p.y * s, p.x * s + p.y * c};

        // Translate the point back to the center
        output += center;

        // Return the rotated point
        return output;
    }

    base_point<T> operator+(const base_point<T> &p) const { return {x + p.x, y + p.y}; }
    base_point<T> operator-(const base_point<T> &p) const { return {x - p.x, y - p.y}; }
    base_point<T> operator*(const base_point<T> &p) const { return {x * p.x, y * p.y}; }
    base_point<T> operator/(const base_point<T> &p) const { return {x / p.x, y / p.y}; }
    base_point<T> operator+(const T &s) const { return {x + s, y + s}; }
    base_point<T> operator-(const T &s) const { return {x - s, y - s}; }
    base_point<T> operator*(const T &s) const { return {x * s, y * s}; }
    base_point<T> operator/(const T &s) const { return {x / s, y / s}; }

    base_point<T> &operator+=(const base_point<T> &p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }
    base_point<T> &operator-=(const base_point<T> &p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }
    base_point<T> &operator*=(const base_point<T> &p)
    {
        x *= p.x;
        y *= p.y;
        return *this;
    }
    base_point<T> &operator/=(const base_point<T> &p)
    {
        x /= p.x;
        y /= p.y;
        return *this;
    }
    base_point<T> &operator+=(const T &s)
    {
        x += s;
        y += s;
        return *this;
    }
    base_point<T> &operator-=(const T &s)
    {
        x -= s;
        y -= s;
        return *this;
    }
    base_point<T> &operator*=(const T &s)
    {
        x *= s;
        y *= s;
        return *this;
    }
    base_point<T> &operator/=(const T &s)
    {
        x /= s;
        y /= s;
        return *this;
    }

    bool operator==(const base_point<T> &p) const { return x == p.x && y == p.y; }
    bool operator!=(const base_point<T> &p) const { return x != p.x || y != p.y; }

    base_point<T> &operator=(const base_point<T> &p)
    {
        x = p.x;
        y = p.y;

        return *this;
    }

    template <class F>
    base_point<T> &operator=(const base_point<F> &p)
    {
        x = (T)p.x;
        y = (T)p.y;

        return *this;
    }

    void write(std::ofstream &out) const
    {
        out.write((char *)&x, d_size);
        out.write((char *)&y, d_size);
    }
    void read(std::ifstream &in)
    {
        in.read((char *)&x, d_size);
        in.read((char *)&y, d_size);
    }

    // template <class F>
    // operator base_point<F>() const { return base_point<F>(*this); }
};
template <class T>
std::ostream &operator<<(std::ostream &os, const base_point<T> &p) { return os << '(' << p.x << ", " << p.y << ')'; }

typedef base_point<int32_t> point2;
typedef base_point<float> point2f;
