#ifndef TYPE_LIST_eb8bebc0d0634a43ac23e4eec45f9329
#define TYPE_LIST_eb8bebc0d0634a43ac23e4eec45f9329

/****************************************************************************
*
*  Description:
*   Used for creating lists of types.
*
*  Author: Zachary Stiers (2015-05-29)
*  Owner:  Zachary Stiers
*
****/

#include <type_traits>
#include "../External/Loki/Typelist.h"

namespace Baseless
{
    namespace Utility
    {
        namespace TypeList
        {
            // Forward list builder from Loki
            template
            <
                typename T1  = Loki::NullType, typename T2  = Loki::NullType, typename T3  = Loki::NullType,
                typename T4  = Loki::NullType, typename T5  = Loki::NullType, typename T6  = Loki::NullType,
                typename T7  = Loki::NullType, typename T8  = Loki::NullType, typename T9  = Loki::NullType,
                typename T10 = Loki::NullType, typename T11 = Loki::NullType, typename T12 = Loki::NullType,
                typename T13 = Loki::NullType, typename T14 = Loki::NullType, typename T15 = Loki::NullType,
                typename T16 = Loki::NullType, typename T17 = Loki::NullType, typename T18 = Loki::NullType
            >
            using List = typename Loki::TL::MakeTypelist<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18>::Result;

            // Forward IndexOf from Loki
            template <class List, class T> 
            using IndexOf = typename Loki::TL::IndexOf<List, T>;

            // My addtions
            template <typename List> struct GetHead                   { typedef typename List::Head Result; };
            template <>              struct GetHead<Loki::NullType>   { typedef Loki::NullType      Result; };

            // Type Matching (uses Pred<T>::IsMatch::value to solve if found)
            typedef Loki::NullType MatchNotFound;
            template <typename Head, typename Pred> struct CheckMatch { static const bool value = Pred::IsMatch<Head>::value; };

            template <typename List, typename Pred, bool found = CheckMatch<typename GetHead<List>::Result, Pred>::value> struct TypeMatching;
            template <typename List, typename Pred>          struct TypeMatching<List, Pred, false>                       { typedef MatchNotFound Result; };
            template <class Head, class Tail, typename Pred> struct TypeMatching<Loki::Typelist<Head, Tail>, Pred, true>  { typedef Head Result; };
            template <class Head, class Tail, typename Pred> struct TypeMatching<Loki::Typelist<Head, Tail>, Pred, false> { typedef typename TypeMatching<Tail, Pred>::Result Result; };

            // Common matcher predictates
            template <typename Base> struct MatchIsDerived { template <typename U> struct IsMatch { static const bool value = std::is_base_of<Base, U>::value; }; };
            template <typename T> struct MatchIsSame       { template <typename U> struct IsMatch { static const bool value = std::is_same<T, U>::value;       }; };
        } // namespace Typelist
    } // namespace Utility
} // namespace Baseless

#endif // Include guard