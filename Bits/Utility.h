#ifndef UTILITY_d4530679c6ae4c758110bc4eff090645
#define UTILITY_d4530679c6ae4c758110bc4eff090645

/****************************************************************************
*
*  Description:
*   Some useful bit operations.
*
*  Author: Zachary Stiers (2015-05-29)
*  Owner:  Zachary Stiers
*
****/

#if defined(_MSC_VER)
#   include <intrin.h>
#endif

#include "..\Debug\Assert.h"
#include <cstdint>

namespace Baseless
{
    namespace Bits
    {
#define SwapXOR(a, b)	                                    (((a) ^ (b)) && ((b) ^= (a) ^= (b), (a) ^= (b)))

        // Atomic Compare and swap
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050
        //untested
#   define CompareAndSwap(outVal, newVal, oldVal)          OSAtomicCompareAndSwapPtr(oldVal, newVal, &outVal)
#elif defined(_MSC_VER)
#   define CompareAndSwap(outVal, newVal, oldVal)          InterlockedCompareExchange(&outVal, newVal, oldVal)
#elif (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) > 40100
        //untested
#   define CompareAndSwap(outVal, newVal, oldVal)         __sync_bool_compare_and_swap(&outVal, oldVal, newVal)
#else
#   error "No atomic compare and swap"
#endif

        inline unsigned char ReverseBits (unsigned char _byte)
        {
	        return (unsigned char)(((_byte * 0x0802LU & 0x22110LU) | (_byte * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16);
        }

        inline int CountBits (unsigned long v)
        {
	        v = v - ((v >> 1) & 0x55555555);								// reuse input as temporary
	        v = (v & 0x33333333) + ((v >> 2) & 0x33333333);					// temp
	        return (int)(((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24);	// count
        }

        // 32 bit version
        template <typename T>
        inline int FindLowestSetBit (T val)
        {
	        return sizeof(T) > sizeof(uint32_t) ? FindLowestSetBit((uint64_t)val) : FindLowestSetBit((uint32_t)val);
        }

        // 32 bit version
        inline int FindLowestSetBit (uint32_t val)
        {
            ASSERT_EXPR(val);
#if defined(_MSC_VER)
	        unsigned long index;
            _BitScanForward(&index, val);
            return index;
#elif defined(__GCC__)
	        return __builtin_ffsl(val) - 1;
#endif
        }

        // 64 bit version
        inline int FindLowestSetBit (uint64_t val)
        {
            ASSERT_EXPR(val);
#if defined(_MSC_VER)
#   if defined(_WIN64)
	        unsigned long index;
            _BitScanForward64(&index, val);
            return index;
#   else
            // TODO : TEST THIS
	        unsigned long index;
            if (!_BitScanForward(&index, unsigned long(val))
            {
                _BitScanForward(&index, unsigned long(val >> 32));
                index |= 32;
            }
            return index;
#   endif
#elif defined(__GCC__)
            // TODO : TEST THIS
	        return __builtin_ffsll(val) - 1;
#endif
        }


        // 32 bit version
        template <typename T>
        inline int FindHighestSetBit (T val)
        {
	        return sizeof(T) > sizeof(unsigned long) ? FindHighestSetBit((uint64_t)val) : FindHighestSetBit((uint32_t)val);
        }

        // 32 bit version
        inline int FindHighestSetBit (uint32_t val)
        {
            ASSERT_EXPR(val);
#if defined(_MSC_VER)
	        unsigned long index;
	        _BitScanReverse(&index, val);
            return index;
#elif defined(__GCC__)
            // TODO : TEST THIS
	        return __builtin_ffsl(val) - 1;
#endif
        }

        // 64 bit version
        inline int FindHighestSetBit (uint64_t val)
        {
            ASSERT_EXPR(val);
#if defined(_MSC_VER)
#   if defined(_WIN64)
	        unsigned long index;
	        _BitScanReverse64(&index, val);
            return index;
#   else
            // TODO : TEST THIS
	        unsigned long index;
            if (_BitScanForward(&index, unsigned long(val >> 32))
                index |= 32;
            else
                _BitScanForward(&index, unsigned long(val));
            return index;
#   endif
#elif defined(__GCC__)
            // TODO : TEST THIS
	        return __builtin_ffsll(val) - 1;
#endif
        }

    } // namesapce Bits
} // namespace Baseless

#endif // Include guard