#ifndef BIT_MASK_aaff6da3d72f49deb5ff8d6f689720e9
#define BIT_MASK_aaff6da3d72f49deb5ff8d6f689720e9

/****************************************************************************
*
*  Description:
*   Easy interface for bit operations. Allows all sorts of fun masking junk.
*
*  Remaining work:
*   - Move Pow2Round macros into a math file
*   - Fixed size masks larger than 64 bits
*   - Variable size masks.
*
*  Author: Zachary Stiers (2015-05-29)
*  Owner:  Zachary Stiers
*
****/

#include "../Utility/Common.h"
#include "Utility.h"
#include <cstdint>
#include <limits>
#include <type_traits>

// TODO : Move to math
template<uint32_t A, uint8_t B = 16>
struct Pow2RoundDown { enum{ value = Pow2RoundDown<(A | (A >> B)), B/2>::value }; };
template<uint32_t A>
struct Pow2RoundDown<A, 1> { enum{ value = (A | (A >> 1)) - ((A | (A >> 1)) >> 1) }; };

template<uint32_t A, uint8_t B = 16>
struct Pow2RoundUp { enum{ value = Pow2RoundUp<((B == 16 ? (A-1) : A) | ((B == 16 ? (A-1) : A) >> B)), B/2>::value }; };
template<uint32_t A >
struct Pow2RoundUp<A, 1> { enum{ value = ((A | (A >> 1)) + 1) }; };

namespace Baseless
{
    namespace Bits
    {
        template <size_t bitCount> struct TBitCountToType;
        template <>                struct TBitCountToType<8>   { typedef uint8_t  Result; };
        template <>                struct TBitCountToType<16>  { typedef uint16_t Result; };
        template <>                struct TBitCountToType<32>  { typedef uint32_t Result; };
        template <>                struct TBitCountToType<64>  { typedef uint64_t Result; };

        template <size_t bitCount> struct TBitCountToSize;
        template <size_t bitCount> struct TBitCountToSize;

        template <size_t bitCount, typename BitType = size_t>
        class TBitMask
        {
            static const size_t BIT_COUNT          = bitCount;
            static const size_t ROUND_UP_BIT_COUNT = Pow2RoundUp<bitCount>::value > 8 ? Pow2RoundUp<bitCount>::value : 8;
            typedef typename TBitCountToType<ROUND_UP_BIT_COUNT>::Result Type;
            Type m_val;

        private: // Helpers
            static inline Type BitToMask (BitType bit) { return static_cast<Type>(1) << bit; }
            template<typename... Args>
            static inline Type BitsToMask (BitType arg1, const Args&... args)  { return BitToMask(arg1) | BitsToMask(args...); }
            static inline Type BitsToMask ()                                   { return 0; }

        public: // Construction
            TBitMask () : m_val(0)           { }
            TBitMask (Type val) : m_val(val) { }

        public: // Basic mutators
            inline void ClearBit (BitType bit)  { ClearMask(BitToMask(bit));  }
            inline void ClearMask (Type mask)   { m_val &= ~mask;             }
            inline void SetBit (BitType bit)    { SetMask(BitToMask(bit));    }
            inline void SetMask (Type mask)     { m_val |= mask;              }
            inline void ToggleBit (BitType bit) { ToggleMask(BitToMask(bit)); }
            inline void ToggleMask (Type mask)  { m_val ^= mask;              }

        public: // Combination mutators
            template<typename... Args> inline void ClearBits (BitType arg1, BitType arg2, const Args&... args)  { ClearMask(BitsToMask(arg1, arg2, args...));  }
            template<typename... Args> inline void SetBits (BitType arg1, BitType arg2, const Args&... args)    { SetMask(BitsToMask(arg1, arg2, args...));    }
            template<typename... Args> inline void ToggleBits (BitType arg1, BitType arg2, const Args&... args) { ToggleMask(BitsToMask(arg1, arg2, args...)); }

        public: // Advanced mutators
            inline void SetBitTo (BitType bit, bool val) { SetMaskTo(BitToMask(bit));             }
            inline void SetMaskTo (Type mask, bool val)  { val ? SetMask(mask) : ClearMask(mask); }

        public: // Some nice accessors
            inline int  Count () const                 { return PopulationCount(m_val);       }
            inline bool CheckBit (BitType bit) const   { return CheckMaskAny(BitToMask(bit)); }
            inline bool CheckMaskAll (Type mask) const { return (m_val & mask) == mask;       }
            inline bool CheckMaskAny (Type mask) const { return (m_val & mask) != 0;          }

        public: // Combination accessors
            template<typename... Args> inline bool CheckBitsAll (BitType arg1, BitType arg2, const Args&... args) const { return CheckMaskAll(BitsToMask(arg1, arg2, args...)); }
            template<typename... Args> inline bool CheckBitsAny (BitType arg1, BitType arg2, const Args&... args) const { return CheckMaskAny(BitsToMask(arg1, arg2, args...)); }
            
        public: // Operators (add non-const versions)
	        inline bool operator [] (BitType bit) const { return CheckBit(bit); }
            inline operator Type () const               { return m_val;         }
            
        public: // Iterators
            class CIteratorConst
            {
                Type m_val;
            public:
                inline CIteratorConst (Type val) : m_val(val) { }
                inline bool operator!= (const CIteratorConst & rhs) const { return m_val != rhs.m_val; }
                inline CIteratorConst & operator++ ()                     { m_val &= ~BitToMask(FindLowestSetBit(m_val)); return *this; }
                inline int operator* () const                             { return FindLowestSetBit(m_val); }
            };

        public: // Iterator functions
            inline CIteratorConst begin () const { return CIteratorConst(m_val); }
            inline CIteratorConst end () const   { return CIteratorConst(0);     }
        };

        // This is intended to handle all the magic of bit masks but for common enum bits.
        template <typename T, size_t bitCount = MAX(T::BIT_COUNT, std::numeric_limits<size_t>::digits)>
        using TBitEnum = TBitMask<bitCount, T>;

    } // namespace Bits
} // namespace Baseless

#endif // Include guard