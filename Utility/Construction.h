#ifndef CONSTRUCTION_6fac222b9ea843598f63c2c5d13f42ae
#define CONSTRUCTION_6fac222b9ea843598f63c2c5d13f42ae

/****************************************************************************
*
*  Description:
*   Used for generic construction and destruction.
*
*  Author: Zachary Stiers (2015-05-29)
*  Owner:  Zachary Stiers
*
****/

#include <new>

namespace Baseless
{
    namespace Utility
    {
        template <typename T>
        inline void Construct (T & place)
        {
            ::new(&place) T();
        }

        template <typename T>
        inline void Construct (T * place, T * stop)
        {
            ::new(place) T[stop - place];
        }

        template <typename T>
        inline void ConstructFrom (T & place, const T & from)
        {
            ::new(&place) T(from);
        }

        template <typename T>
        inline void ConstructFrom (T * place, T * stop, const T & from)
        {
            for (; place != stop; ++place)
                ::new(place) T(from);
        }

        template <typename T>
        inline void ConstructFrom (T & place, T && from)
        {
            ::new(&place) T(std::forward<T>(from));
        }

        template <typename T>
        inline void ConstructFrom (T * place, T * stop, T && from)
        {
            for (; place != stop; ++place)
                ::new(place) T(std::forward<T>(from));
        }

        template <typename T>
        inline void Destruct (const T & place)
        {
            place.~T();
        }

        template <typename T>
        inline void Destruct (const T * place, const T * stop)
        {
            for (; place != stop; ++place)
                place->~T();
        }
    } // namespace Utility
} // namespace Baseless

#endif // Include guard