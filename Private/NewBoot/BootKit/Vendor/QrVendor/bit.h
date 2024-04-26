#ifndef UTL_BIT_H
#define UTL_BIT_H

#include <bit>

namespace utl {

/**
 * @brief Size of object in terms of bits.
 *
 * @tparam T Object type
 * @return Number of bits
 */
template<class T>
constexpr auto bit_size()
{
    return sizeof(T) * 8;
}

/**
 * @brief Integer with all bits set to 1.
 *
 * @tparam T Integer type
 * @return All set integer
 */
template<class T>
constexpr T bit_full()
{
    return T(-1);
}

/**
 * @brief Wrap around mask for power of two number of bits
 * within given integer type. For example:
 * [ bit_wrap<uint8_t>  = 8  - 1 = 0b111   ]
 * [ bit_wrap<uint16_t> = 16 - 1 = 0b1111  ]
 * [ bit_wrap<uint32_t> = 32 - 1 = 0b11111 ]
 *
 * @tparam T Integer type
 * @return Wrap around mask for number of bits
 */
template<class T>
constexpr T bit_wrap()
{
    return bit_size<T>() - 1;
}

/**
 * @brief Number of bits to fit bit_wrap<T> result, in other words
 * bit width of (sizeof(T) * 8 - 1). For example:
 * [ bit_shft<uint8_t>  = bit_width(0b111)      = 3 ]
 * [ bit_shft<uint16_t> = bit_width(0b1111)     = 4 ]
 * [ bit_shft<uint32_t> = bit_width(0b11111)    = 5 ]
 *
 * @tparam T Integer type
 * @return Number of bits to shift to divide by sizeof(T) * 8
 */
template<class T>
constexpr auto bit_shft()
{
    return std::bit_width(bit_wrap<T>());
}

/**
 * @brief Round up division by number of bits within given integer type,
 * which sizeof(T) * 8 is power of two.
 *
 * @tparam T Inetegr type
 * @param x Dividend
 * @return Quotient
 */
template<class T>
constexpr auto bit_ceil(auto x)
{
    return (x + bit_wrap<T>()) >> bit_shft<T>();
}

/**
 * @brief Count leading zeros.
 *
 * @param x Unsigned integer argument
 * @return Number of leading zeros
 */
constexpr unsigned cntlz(auto x)
{
    if constexpr (std::is_same_v<decltype(x), int>)
        return std::countl_zero(unsigned(x));
    else
        return std::countl_zero(x);
}

/**
 * @brief Count trailing zeros.
 *
 * @param x Unsigned integer argument
 * @return Number of trailing zeros
 */
constexpr unsigned cnttz(auto x)
{
    if constexpr (std::is_same_v<decltype(x), int>)
        return std::countr_zero(unsigned(x));
    else
        return std::countr_zero(x);
}

/**
 * @brief Get number of words (integers) required to store N bits.
 *
 * @tparam T Word integer type
 * @param n Number of bits to store
 * @return Number of words
 */
template<class T>
constexpr size_t words_in_bits(size_t n)
{
    return (n >> bit_shft<T>()) + !!(n & bit_wrap<T>());
}

/**
 * @brief Get number of bytes required to store N bits.
 *
 * @param n Number of bits to store
 * @return Number of bytes
 */
constexpr size_t bytes_in_bits(size_t n)
{
    return words_in_bits<uint8_t>(n);
}

/**
 * @brief Make integer with bit at given position.
 *
 * @tparam T Inetegr type
 * @param n Bit position
 * @return Integer with set bit
 */
template<class T = unsigned>
constexpr T bit(int n)
{
    return T(1) << n;
}

/**
 * @brief Get n-th bit of an integer.
 *
 * @tparam T Integer type
 * @param x Integer
 * @param n Bit position from LSB
 * @return true if set
 */
template<class T>
constexpr bool get_bit(T x, int n)
{
    return (x >> n) & 1;
}

/**
 * @brief Set n-th bit of an integer.
 *
 * @tparam T Integer type
 * @param x Integer
 * @param n Bit position from LSB
 */
template<class T>
constexpr void set_bit(T& x, int n)
{
    x |= 1 << n;
}

/**
 * @brief Clear n-th bit of an integer.
 *
 * @tparam T Integer type
 * @param x Integer
 * @param n Bit position from LSB
 */
template<class T>
constexpr void clr_bit(T& x, int n)
{
    x &= ~(1 << n);
}

/**
 * @brief Get n-th bit in array of words (starting from LSB).
 *
 * @tparam T Word type
 * @param p Array of words
 * @param n Index of bit to get
 * @return true if set
 */
template<class T>
constexpr bool get_arr_bit(const T* p, unsigned n)
{
    return get_bit(p[n >> bit_shft<T>()], n & bit_wrap<T>());
}

/**
 * @brief Set n-th bit in array of words (starting from LSB).
 *
 * @tparam T Word type
 * @param p Array of words
 * @param n Index of bit to set
 */
template<class T>
constexpr void set_arr_bit(T* p, unsigned n)
{
    set_bit(p[n >> bit_shft<T>()], n & bit_wrap<T>());
}

/**
 * @brief Clear n-th bit in array of words (starting from LSB).
 *
 * @tparam T Word type
 * @param p Array of words
 * @param n Index of bit to clear
 */
template<class T>
constexpr void clr_arr_bit(T* p, unsigned n)
{
    clr_bit(p[n >> bit_shft<T>()], n & bit_wrap<T>());
}

/**
 * @brief Shift bits left in array of integer elements from least significant bit
 * and considering 0-th byte as the right most.
 * uint16_t example: 0b10000000'11100001 ==> 0b00000001'11000010.
 *
 * @tparam T Integer type
 * @tparam L Length of array
 * @param x Array of integers, nullptr not acceptable!
 * @param len Number of elements
 */
template<class T, size_t L>
constexpr void shift_left(T (&x)[L])
{
    for (int i = L - 1; i > 0; --i) {
        x[i] <<= 1;
        x[i] |= x[i - 1] >> bit_wrap<T>();
    }
    x[0] <<= 1;
}

}

#endif
