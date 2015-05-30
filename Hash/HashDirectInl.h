#ifndef HASH_DIRECT_9f277fdebc6c4dbe986cc84912582573
#define HASH_DIRECT_9f277fdebc6c4dbe986cc84912582573

/****************************************************************************
*
*  Description:
*   Defines the basic implementations for an extendable HashDirect function.
*
*  Notes:
*   HashDirect may take any arguments and does not need to be templated.
*   The fallback behavior of HashDirect is a single HashAppend
*
*  Author: Zachary Stiers (2015-05-22)
*  Owner:  Zachary Stiers
*
****/

// This file is included by Hash.h

namespace Baseless
{
    namespace Hash
    {
        // Basic
        template <typename T, bool valid = std::is_enum<T>::value || std::is_fundamental<T>::value || std::is_pointer<T>::value>
        struct HashDirectControllerBasic : HashControllerValid
        {
            template <typename H>
            static inline HashCode HashDirect (T val)
            {
                return H::HashDirect(&val, sizeof(val));
            }

            template <typename H, typename U>
            static inline HashCode HashDirect (T val, const U & seed)
            {
                return H::HashDirect(&val, sizeof(val), seed);
            }
        };

        template <typename T>
        struct HashDirectControllerBasic <T, false> { };

        // Float
        template <typename T, bool valid = std::is_floating_point<T>::value>
        struct HashDirectControllerFloat : HashControllerValid
        {
            template <typename H>
            static inline HashCode HashDirect (T val)
            {
                if (!val)
                    val = 0;
                return H::HashDirect(&val, sizeof(val));
            }

            template <typename H, typename U>
            static inline HashCode HashDirect (T val, const U & seed)
            {
                if (!val)
                    val = 0;
                return H::HashDirect(&val, sizeof(val), seed);
            }
        };

        template <typename T>
        struct HashDirectControllerFloat <T, false> { };

        // This should be valid for all types
        template <typename T>
        struct HashDirectControllerAny : HashControllerValid
        {
            template <typename H>
            static inline HashCode HashDirect (const T & val)
            {
                H hasher; hasher.Append(val); return hasher.Result();
            }

            template <typename H, typename R, typename U>
            static inline HashCode HashDirect (const T & val, const U & seed)
            {
                H hasher(seed); hasher.Append(val); return hasher.Result();
            }
        };

        template <typename T>
        struct HashDirectController {
            typedef Utility::TypeList::List<HashDirectControllerFloat<T>, HashDirectControllerBasic<T>, HashDirectControllerAny<T>> ControllerList;

            // TListSearch searches linearly though the list until the first child deriving from RequiredBase is encountered.
            typedef typename Utility::TypeList::TypeMatching<ControllerList, Utility::TypeList::MatchIsDerived<HashControllerValid>>::Result CorrectController;
        
            template <typename H>
            static inline HashCode HashDirect (const T & val)
            {
                static_assert(!std::is_same<CorrectController, TypeList::MatchNotFound>::value, "No valid hash function exists for type. Check output for type information.");
                return CorrectController::HashDirect<H>(val);
            }
        
            template <typename H, typename U>
            static inline HashCode HashDirect (const T & val, const U & seed)
            {
                static_assert(!std::is_same<CorrectController, TypeList::MatchNotFound>::value, "No valid hash function exists for type. Check output for type information.");
                return CorrectController::HashDirect<H, U>(val, seed);
            }
        };

        template <typename H, typename T>
        HashCode HashDirect (const T & val)
        {
            return HashDirectController<T>::HashDirect<H>(val);
        }

        template <typename H, typename T, typename U>
        HashCode HashDirect (const T & val, const U & seed)
        {
            return HashDirectController<T>::HashDirect<H, U>(val, seed);
        }
    }
}

#endif // Include guard