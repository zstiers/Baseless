#ifndef BIT_MASK_aaff6da3d72f49deb5ff8d6f689720e9
#define BIT_MASK_aaff6da3d72f49deb5ff8d6f689720e9

/****************************************************************************
*
*  Description:
*   Easy interface for bit operations. Allows all sorts of fun masking junk.
*
*  Author: Zachary Stiers (2015-05-29)
*  Owner:  Zachary Stiers
*
****/

#include "Utility.h"
#include <cstdint>
#include <limits>

namespace Baseless
{
    namespace Bits
    {
        template <size_t bitCount> struct TBitCountToType;
        template <>                struct TBitCountToType<8>   { typedef uint8_t  Result; };
        template <>                struct TBitCountToType<16>  { typedef uint16_t Result; };
        template <>                struct TBitCountToType<32>  { typedef uint32_t Result; };
        template <>                struct TBitCountToType<64>  { typedef uint64_t Result; };

        template <size_t bitCount>
        class TBitMask
        {
            typedef typename TBitCountToType<bitCount>::Result Type;
            Type m_val;

        private: // Helpers
            static inline Type BitToMask (size_t bit) { return static_cast<Type>(1) << bit; }

        public: // Construction
            TBitMask () : m_val(0)           { }
            TBitMask (Type val) : m_val(val) { }

        public: // Basic mutators
            inline void ClearBit (size_t bit)  { ClearMask(BitToMask(bit));  }
            inline void ClearMask (Type mask)  { m_val &= ~mask;             }
            inline void SetBit (size_t bit)    { SetMask(BitToMask(bit));    }
            inline void SetMask (Type mask)    { m_val |= mask;              }
            inline void ToggleBit (size_t bit) { ToggleMask(BitToMask(bit)); }
            inline void ToggleMask (Type mask) { m_val ^= mask;              }

        public: // Advanced mutators
            inline void SetBitTo (size_t bit, bool val) { SetMaskTo(BitToMask(bit));             }
            inline void SetMaskTo (Type mask, bool val) { val ? SetMask(mask) : ClearMask(mask); }

        public: // Some nice accessors
            inline bool CheckBit (size_t bit)    { return CheckMaskAny(BitToMask(bit)); }
            inline bool CheckMaskAll (Type mask) { return (m_val & mask) == mask;       }
            inline bool CheckMaskAny (Type mask) { return (m_val & mask) != 0;          }
            
        public: // Operators (add non-const versions)
	        inline bool operator [] (size_t bit) const { return CheckBit(bit); }
            inline operator Type () const              { return m_val;         }
            
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

    } // namespace Bits
} // namespace Baseless

#endif // Include guard