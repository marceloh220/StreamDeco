#ifndef __CONST_USER_HPP__
#define __CONST_USER_HPP__

#include "esp_heap_caps.h"

/**
 * @brief   User-defined literal for time units in Herts
 * @param   x Value in Herts
 * @return  Value in Herts
 */
constexpr unsigned long long operator"" _Hz(unsigned long long x)
{
    return x * 1;
}

/**
 * @brief   User-defined literal for time units in kiloHerts
 * @param   x Value in kiloHerts
 * @return  Value in Herts
 */
constexpr unsigned long long operator"" _kHz(unsigned long long x)
{
    return x * 1000;
}

/**
 * @brief   User-defined literal for time units in megaHerts
 * @param   x Value in megaHerts
 * @return  Value in Herts
 */
constexpr unsigned long long operator"" _MHz(unsigned long long x)
{
    return x * 1000 * 1000;
}

/**
 * @brief   User-defined literal for time units in gigaHerts
 * @param   x Value in gigaHerts
 * @return  Value in Herts
 */
constexpr unsigned long long operator"" _GHz(unsigned long long x)
{
    return x * 1000 * 1000 * 1000;
}

/**
 * @brief   User-defined literal for memory units in kiloBits
 * @param   x Value in kiloBits
 * @return  Value in Bits
 */
constexpr unsigned long long operator"" _kBit(unsigned long long x)
{
    return x * 1024;
}

/**
 * @brief   User-defined literal for memory units in megaBits
 * @param   x Value in megaBits
 * @return  Value in Bits
 */
constexpr unsigned long long operator"" _MBit(unsigned long long x)
{
    return x * 1024 * 1024;
}

/**
 * @brief   User-defined literal for memory units in gigaBits
 * @param   x Value in gigaBits
 * @return  Value in Bits
 */
constexpr unsigned long long operator"" _GBit(unsigned long long x)
{
    return x * 1024 * 1024 * 1024;
}

/**
 * @brief   User-defined literal for memory units in kiloBytes
 * @param   x Value in kiloBytes
 * @return  Value in Bytes
 */
constexpr unsigned long long operator"" _kB(unsigned long long x)
{
    return x * 1024;
}

/**
 * @brief   User-defined literal for memory units in megaBytes
 * @param   x Value in megaBytes
 * @return  Value in Bytes
 */
constexpr unsigned long long operator"" _MB(unsigned long long x)
{
    return x * 1024 * 1024;
}

/**
 * @brief   User-defined literal for memory units in gigaBytes
 * @param   x Value in gigaBytes
 * @return  Value in Bytes
 */
constexpr unsigned long long operator"" _GB(unsigned long long x)
{
    return x * 1024 * 1024 * 1024;
}

/**
 * @brief   Memory management namespace
 */
namespace memory
{

    /**
     * @brief   Allocate memory for a single object of type T
     * @param   caps Memory capabilities flags
     * @return  Pointer to allocated object of type T
     * @note   Uses heap_caps_malloc from esp_heap_caps.h
     *         Example:
     *     @code
     *      MyClass *obj = memory::alloc<MyClass>(MALLOC_CAP_8BIT);
     *     @endcode
     */
    template <typename T>
    T *alloc(uint32_t caps)
    {
        return static_cast<T *>(heap_caps_malloc(sizeof(T), caps));
    }

    /**
     * @brief   Allocate memory for an array of objects of type T
     * @param   size Number of objects to allocate
     * @param   caps Memory capabilities flags
     * @return  Pointer to allocated array of objects of type T
     * @note   Uses heap_caps_calloc from esp_heap_caps.h
     *         Example:
     *     @code
     *      MyClass *arr = memory::alloc<MyClass>(10, MALLOC_CAP_8BIT);
     *     @endcode
     */
    template <typename T>
    T *alloc(size_t size, uint32_t caps)
    {
        return static_cast<T *>(heap_caps_calloc(size, sizeof(T), caps));
    }

    /**
     * @brief   Free memory allocated for an object or array of objects of type T
     * @param   pointer Pointer to the object or array to free
     * @note   Uses heap_caps_free from esp_heap_caps.h
     *         Example:
     *     @code
     *      memory::free(obj);
     *      memory::free(arr);
     *     @endcode
     */
    template <typename T>
    void free(T *pointer)
    {
        heap_caps_free(static_cast<void *>(pointer));
    }

} //

/**
 * @brief   Mathematical operations namespace
 */
namespace math
{

    /**
     * @brief   Mathematical constant Pi
     */
    constexpr float pi = 3.14159f;

    /**
     * @brief   Mathematical constant e
     */
    constexpr float e = 2.71828f;

    /**
     * @brief   Map a value from one range to another
     * @param   x Value to be mapped
     * @param   input_min Minimum of input range
     * @param   input_max Maximum of input range
     * @param   output_min Minimum of output range
     * @param   output_max Maximum of output range
     * @return  Mapped value in output range
     */
    template <typename T>
    T map(T x, T input_min, T input_max, T output_min, T output_max)
    {
        return (x - input_min) * (output_max - output_min) / (input_max - input_min) + output_min;
    }

    /**
     * @brief   Get maximum of two values
     * @param   a First value
     * @param   b Second value
     * @return  Maximum value
     */
    template <typename T>
    T max(T a, T b) { return a > b ? a : b; }

    /**
     * @brief   Get minimum of two values
     * @param   a First value
     * @param   b Second value
     * @return  Minimum value
     */
    template <typename T>
    T min(T a, T b) { return a < b ? a : b; }

    /**
     * @brief   Swap two values
     * @param   a First value
     * @param   b Second value
     */
    template <typename T>
    void swap(T &a, T &b)
    {
        T temp = a;
        a = b;
        b = temp;
    }

    /**
     * @brief   Clamp a value between a minimum and maximum
     * @param   val Value to be clamped
     * @param   min Minimum value
     * @param   max Maximum value
     */
    template <typename T>
    void clamp(T &val, T min, T max)
    {
        if (val < min)
            val = min;
        if (val > max)
            val = max;
    }

} // namespace math

/**
 * @brief   Bitwise operations namespace
 */
namespace bitwise
{

    /**
     * @brief   Get bitmask for a specific bit
     * @param   _bit Bit position
     * @return  Bitmask with only the specified bit set
     */
    constexpr unsigned int bitmask(unsigned int _bit) { return (1 << _bit); }

    /**
     * @brief   Clear a specific bit in a SFR
     * @param   sfr Reference to SFR
     * @param   _bit Bit position to clear
     */
    constexpr void clear(unsigned int &sfr, unsigned int _bit) { sfr &= ~bitmask(_bit); }

    /**
     * @brief   Set a specific bit in a SFR
     * @param   sfr Reference to SFR
     * @param   _bit Bit position to set
     */
    constexpr void set(unsigned int &sfr, unsigned int _bit) { sfr |= bitmask(_bit); }

    /**
     * @brief   Toggle a specific bit in a SFR
     * @param   sfr Reference to SFR
     * @param   _bit Bit position to toggle
     */
    constexpr void toogle(unsigned int &sfr, unsigned int _bit) { sfr ^= bitmask(_bit); }

    /**
     * @brief   Test if a specific bit is set in a SFR
     * @param   sfr Reference to SFR
     * @param   _bit Bit position to test
     * @return  True if the bit is set, false otherwise
     */
    constexpr bool test(unsigned int &sfr, unsigned int _bit) { return sfr & bitmask(_bit) ? true : false; }

    /**
     * @brief   Get low byte from a 16-bit data
     * @param   data 16-bit data
     * @return  Low byte
     */
    constexpr uint8_t low(uint16_t data) { return static_cast<uint8_t>((data >> 0) & 0xFF); }

    /**
     * @brief   Get high byte from a 16-bit data
     * @param   data 16-bit data
     * @return  High byte
     */
    constexpr uint8_t high(uint16_t data) { return static_cast<uint8_t>((data >> 8) & 0xFF); }

} // namespace bitwise

#endif
