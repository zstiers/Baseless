#ifndef XXHASH_94240f6a16d643aa8635ddff59f1ef8d
#define XXHASH_94240f6a16d643aa8635ddff59f1ef8d

/****************************************************************************
*
*  Description:
*   Used for wrapping the xxHash algorithm in the IHasher interface
*
*  Notes:
*   This file does not need to be defined inline like this, but is done so for optimization.
*
*  Author: Zachary Stiers (2015-05-22)
*  Owner:  Zachary Stiers
*
****/

#include "Hash.h"
#include "../External/xxHash/xxhash.h"

namespace Baseless
{
    namespace Hash
    {
        class xxHash32 :
            public Hash::IHasher,
            public Hash::InlineHasher<xxHash32>
        {
            XXH32_state_t state;

        public: // Useful types and values
            typedef unsigned SeedType;
            static const SeedType DefaultSeed = 0;

        public: // Construct
            xxHash32 (SeedType seed = DefaultSeed) { XXH32_reset(&state, seed); }

        public: // IHasher
            void AppendBytes (const void * buffer, size_t byteCount) override { XXH32_update(&state, buffer, byteCount); }
            HashCode Result () const override                                 { return HashCode(XXH32_digest(&state));             }

        public: // Optimized pretty access for special types. Will clean this up later.
            static HashCode HashDirect (const void * data, size_t byteCount, SeedType seed = DefaultSeed) { return HashCode(XXH32(data, byteCount, seed)); }
        };


        class xxHash64 :
            public Hash::IHasher,
            public Hash::InlineHasher<xxHash64>
        {
            XXH64_state_t state;

        public: // Useful types and values
            typedef unsigned long long SeedType;
            static const SeedType DefaultSeed = 0;

        public: // Construct
            xxHash64 (SeedType seed = DefaultSeed) { XXH64_reset(&state, seed); }

        public: // IHasher
            void AppendBytes (const void * buffer, size_t byteCount) override { XXH64_update(&state, buffer, byteCount); }
            HashCode Result () const override                                 { return HashCode(XXH64_digest(&state));   }

        public: // Optimized pretty access for special types. Will clean this up later.
            static HashCode HashDirect (const void * data, size_t byteCount, SeedType seed = DefaultSeed) { return HashCode(XXH64(data, byteCount, seed)); }
        };

        // Uses pointer-size selection in order to pick the most optimized version.
        template <size_t> struct xxHashPicker;
        template <>       struct xxHashPicker<4> { typedef xxHash32 Result; };
        template <>       struct xxHashPicker<8> { typedef xxHash64 Result; };
        typedef xxHashPicker<sizeof(size_t)>::Result xxHash;

    } // namespace Hash
} // namespace Baseless

#endif // Include guard