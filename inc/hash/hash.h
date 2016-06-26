#ifndef HASH_d3ab7dda675f4ff3ab6b6297c61b1773
#define HASH_d3ab7dda675f4ff3ab6b6297c61b1773

/****************************************************************************
*
*  Description:
*   Used for defining a reusable hashing interface.
*
*  Notes:
*   N/A
*
*  Author: Zachary Stiers (2015-05-22)
*  Owner:  Zachary Stiers
*
****/

#include "../Utility/TypeList.h"
#include <type_traits>

namespace Baseless
{
    namespace Hash
    {
        typedef size_t HashCode;

        class IHasher
        {
        public: // Exposing here for usefulness
            typedef HashCode HashCode;

        public:
            virtual void     AppendBytes (const void * buffer, size_t byteCount) = 0;
            virtual HashCode Result () const = 0;

        public: // Templated neatness
            template <typename T>
            void Append (const T & val) { HashAppend(*this, val); }
            template <typename T>
            void AppendBytes (const T & val) { AppendBytes(&val, sizeof(T)); }
        };

        template <typename H>
        class InlineHasher
        {
        public: // More pretty access
            template <typename V>
            static HashCode Hash (const V & val)                 { return HashDirect<H>(val); }
            template <typename V, typename W>
            static HashCode Hash (const V & val, const W & seed) { return HashDirect<H>(val, seed); }
        };


        // Valid. Required for searching.
        struct HashControllerValid { };
    }
}

#include "HashAppendInl.h"
#include "HashDirectInl.h"

#endif // Include guard