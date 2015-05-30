#ifndef BEST_LV_d077931e0aa54cf48fc409ee4c76cb43
#define BEST_LV_d077931e0aa54cf48fc409ee4c76cb43

/****************************************************************************
*
*  Description:
*   Determines approrpriate l-values for optimization.
*
*  Notes:
*   This can have weird results when combined with determining function overloads.
*   This is meant to provide low-level optimization in common code, such as containers.
*
*  Author: Zachary Stiers (2015-05-22)
*  Owner:  Zachary Stiers
*
****/

#include <type_traits>

namespace Baseless
{
    namespace Utility
    {
        template <typename T, bool useValue>
        struct BestLvInternal
        {
            typedef T Result;
        };

        template <typename T>
        struct BestLvInternal<T, false>
        {
            typedef const T & Result;
        };

        template <typename T>
        struct BestLv
        {
            static const bool USE_VALUE = std::is_fundamental<T>::value || std::is_pointer<T>::value || std::is_enum<T>::value;
            typedef typename BestLvInternal<T, USE_VALUE>::Result Result;
        };
    }
}

#endif // Include guard