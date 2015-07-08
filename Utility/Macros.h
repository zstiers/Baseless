/****************************************************************************
*
*  Description:
*   A collection of common macros.
*
*  Notes:
*   - The list of macros is alphabetized.
*   - Each macro has its own include protection
*
*  Author: Zachary Stiers (2015-07-07)
*  Owner:  Zachary Stiers
*
****/

/****************************************************************************
*
*  Macro comparisons
*
****/

#if !defined(MAX)
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#if !defined(MIN)
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif


/****************************************************************************
*
*  Remove warnings on unreferenced parameters
*
****/

#if !defined(REF)
#   define REF(...) ((void)(0, __VA_ARGS__))
#endif