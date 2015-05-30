#ifndef SWAP_8bcf96de73a8492f9d708febb42b3a71
#define SWAP_8bcf96de73a8492f9d708febb42b3a71

/****************************************************************************
*
*  Description:
*   Just adds a basic swap function. No magic here.
*
*  Author: Zachary Stiers (2015-05-29)
*  Owner:  Zachary Stiers
*
****/

#include <utility>

namespace Baseless
{
    namespace Utility
    {
        template <typename T>
        void Swap (T & lhs, T & rhs)
        {
            std::swap(lhs, rhs);
        }
    }
}

#endif // Include guard