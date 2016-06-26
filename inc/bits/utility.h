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
#include <climits>

#define TRY_USE_INTRINSICS 1

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

        template <typename T>
        inline int PopulationCount (T v)
        {
#if TRY_USE_INTRINSICS
	        return sizeof(T) > sizeof(uint32_t) ? PopulationCountIntrinsic((uint64_t)v) : PopulationCountIntrinsic((uint32_t)v);
#else
            // The implementation of this function came from http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
            v = v - ((v >> 1) & (T)~(T)0/3);                                // temp
            v = (v & (T)~(T)0/15*3) + ((v >> 2) & (T)~(T)0/15*3);           // temp
            v = (v + (v >> 4)) & (T)~(T)0/255*15;                           // temp
            return (T)(v * ((T)~(T)0/255)) >> (sizeof(T) - 1) * CHAR_BIT; // count
#endif
        }

        inline int PopulationCountIntrinsic (uint32_t v)
        {
#if defined(_MSC_VER)
            return __popcnt((unsigned)v);
#elif defined(__GCC__)
            return __builtin_popcount(v);
#endif
        }

        inline int PopulationCountIntrinsic (uint64_t v)
        {
#if defined(_MSC_VER)
#   if defined(_WIN64)
            return (int)__popcnt64(v);
#   else
            return __popcnt((uint32_t)v) + __popcnt((uint32_t)(v >> 32));;
#   endif
#elif defined(__GCC__)
            return __builtin_popcountll(v);
#endif
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

#if TRY_USE_INTRINSICS && defined(_MSC_VER)
	        unsigned long index;
            _BitScanForward(&index, val);
            return index;
#elif TRY_USE_INTRINSICS && defined(__GCC__)
	        return __builtin_ffsl(val) - 1;
#else
            // TODO : Implement this
#endif
        }

        // 64 bit version
        inline int FindLowestSetBit (uint64_t val)
        {
            ASSERT_EXPR(val);
            
#if TRY_USE_INTRINSICS && defined(_MSC_VER)
#   if defined(_WIN64)
	        unsigned long index;
            _BitScanForward64(&index, val);
            return index;
#   else
            // TODO : TEST THIS
	        unsigned long index;
            if (!_BitScanForward(&index, unsigned long(val)))
            {
                _BitScanForward(&index, unsigned long(val >> 32));
                index |= 32;
            }
            return index;
#   endif
#elif TRY_USE_INTRINSICS && defined(__GCC__)
            // TODO : TEST THIS
	        return __builtin_ffsll(val) - 1;
#else
            // TODO : Implement this
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
#if TRY_USE_INTRINSICS && defined(_MSC_VER)
	        unsigned long index;
	        _BitScanReverse(&index, val);
            return index;
#elif TRY_USE_INTRINSICS && defined(__GCC__)
            // TODO : TEST THIS
	        return __builtin_ffsl(val) - 1;
#else
            // Implement this
#endif
        }

        // 64 bit version
        inline int FindHighestSetBit (uint64_t val)
        {
            ASSERT_EXPR(val);
#if TRY_USE_INTRINSICS && defined(_MSC_VER)
#   if defined(_WIN64)
	        unsigned long index;
	        _BitScanReverse64(&index, val);
            return index;
#   else
            // TODO : TEST THIS
	        unsigned long index;
            if (_BitScanForward(&index, unsigned long(val >> 32)))
                index |= 32;
            else
                _BitScanForward(&index, unsigned long(val));
            return index;
#   endif
#elif TRY_USE_INTRINSICS && defined(__GCC__)
            // TODO : TEST THIS
	        return __builtin_ffsll(val) - 1;
#else
            // TODO : Implement this
#endif
        }

    } // namesapce Bits
} // namespace Baseless

#endif // Include guard