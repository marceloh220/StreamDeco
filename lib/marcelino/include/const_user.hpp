#ifndef __CONST_USER_HPP__
#define __CONST_USER_HPP__

#include "esp_heap_caps.h"

constexpr unsigned long long operator"" _Hz(unsigned long long x)
{
    return x * 1;
}

constexpr unsigned long long operator"" _kHz(unsigned long long x)
{
    return x * 1000;
}

constexpr unsigned long long operator"" _MHz(unsigned long long x)
{
    return x * 1000 * 1000;
}

constexpr unsigned long long operator"" _GHz(unsigned long long x)
{
    return x * 1000 * 1000 * 1000;
}

constexpr unsigned long long operator"" _kBit(unsigned long long x)
{
    return x * 1024;
}

constexpr unsigned long long operator"" _MBit(unsigned long long x)
{
    return x * 1024 * 1024;
}

constexpr unsigned long long operator"" _GBit(unsigned long long x)
{
    return x * 1024 * 1024 * 1024;
}

constexpr unsigned long long operator"" _kB(unsigned long long x)
{
    return x * 1024;
}

constexpr unsigned long long operator"" _MB(unsigned long long x)
{
    return x * 1024 * 1024;
}

constexpr unsigned long long operator"" _GB(unsigned long long x)
{
    return x * 1024 * 1024 * 1024;
}

namespace memory
{

    template <typename T>
    T *alloc(uint32_t caps)
    {
        return static_cast<T *>(heap_caps_malloc(sizeof(T), caps));
    }

    template <typename T>
    T *alloc(size_t size, uint32_t caps)
    {
        return static_cast<T *>(heap_caps_calloc(size, sizeof(T), caps));
    }

    template <typename T>
    void free(T *pointer)
    {
        heap_caps_free(static_cast<void *>(pointer));
    }

} //

namespace math
{

    constexpr float pi = 3.14159f;
    constexpr float e = 2.71828f;

    template <typename T>
    T map(T x, T in_min, T in_max, T out_min, T out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    template <typename T>
    T max(T a, T b) { return a > b ? a : b; }

    template <typename T>
    T min(T a, T b) { return a < b ? a : b; }

    template <typename T>
    void swap(T &a, T &b)
    {
        T temp = a;
        a = b;
        b = temp;
    }

} // namespace math

namespace bitwise
{

    constexpr unsigned int bitmask(unsigned int _bit) { return (1 << _bit); }
    constexpr void clear(unsigned int &sfr, unsigned int _bit) { sfr &= ~bitmask(_bit); }
    constexpr void set(unsigned int &sfr, unsigned int _bit) { sfr |= bitmask(_bit); }
    constexpr void toogle(unsigned int &sfr, unsigned int _bit) { sfr ^= bitmask(_bit); }
    constexpr bool test(unsigned int &sfr, unsigned int _bit) { return sfr & bitmask(_bit) ? true : false; }
    constexpr uint8_t low(uint16_t data) { return static_cast<uint8_t>((data >> 0) & 0xFF); }
    constexpr uint8_t high(uint16_t data) { return static_cast<uint8_t>((data >> 8) & 0xFF); }

} // namespace bitwise

#endif
