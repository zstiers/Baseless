#ifndef HASH_APPEND_80dfbafb16cc4d3da3ba04250054900c
#define HASH_APPEND_80dfbafb16cc4d3da3ba04250054900c

/****************************************************************************
*
*  Description:
*   Defines the basic implementations for an extendable HashAppend function.
*
*  Notes:
*   HashAppend may take any arguments and does not need to be templated.
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
        struct HashAppendControllerBasic : HashControllerValid
        {
            template <typename H>
            static inline void HashAppend (H & hasher, T val)
            {
                hasher.AppendBytes(val);
            }
        };

        template <typename T>
        struct HashAppendControllerBasic <T, false> { };

        // Float
        template <typename T, bool valid = std::is_floating_point<T>::value>
        struct HashAppendControllerFloat : HashControllerValid
        {
            template <typename H>
            static inline void HashAppend (H & hasher, T val)
            {
                if (!val)
                    val = 0;
                hasher.AppendBytes(val);
            }
        };

        template <typename T>
        struct HashAppendControllerFloat <T, false> { };

        // Controller picker
        template <typename T>
        struct HashAppendController {
            typedef Utility::TypeList::List<HashAppendControllerFloat<T>, HashAppendControllerBasic<T>> ControllerList;

            // TListSearch searches linearly though the list until the first child deriving from RequiredBase is encountered.
            typedef typename Utility::TypeList::TypeMatching<ControllerList, Utility::TypeList::MatchIsDerived<HashControllerValid>>::Result CorrectController;

            template <typename H>
            static inline void HashAppend (H & hasher, const T & val)
            {
                static_assert(!std::is_same<CorrectController, Utility::TypeList::MatchNotFound>::value, "No valid hash function exists for type. Check output for type information.");
                return CorrectController::HashAppend(hasher, val);
            }
        };

        template <typename H, typename T>
        void HashAppend (H & hasher, const T & val)
        {
            return HashAppendController<T>::HashAppend(hasher, val);
        }

        template <typename H, typename T, size_t N>
        void HashAppend (H & hasher, const T (& val)[N])
        {
            for (auto ptr = val, term = ptr + N; ptr != term; ++ptr)
                HashAppend(hasher, *ptr);
        }
    }
}

#endif // Include guard