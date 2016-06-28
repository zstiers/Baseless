#include "01_bits\01_bits.h"
#include "02_array\02_array.h"
#include <iostream>
#include <utility\common.h>

typedef void (*FuncTest)(ETestType);

static const FuncTest s_tests[] = {
    &BitsTest,
};

bool RunTest ()
{
    int testIndex = 0;
    std::cin >> testIndex;
    --testIndex;
    if (testIndex < 0)
        return false;

    if (testIndex >= (int)countof(s_tests))
        return true;

    // Skip the .
    std::cin.get();

    int testType = 0;
    std::cin >> testType;
    if (testType >= TEST_TYPES)
        return true;

    s_tests[testIndex](ETestType(testType));
    return true;
}

int main ()
{
    while (RunTest());
    return 1;
}