#include "01_bits.h"
#include "bits\bitmask.h"
#include <iostream>

namespace
{
    void Output (const char str[], bool success)
    {
        std::cout << str << ": " << (success ? "Success" : "Failure") << std::endl;
    }

    void TestCorrectness ()
    {
        // Set some bits and then check them.
        {
            Baseless::Bits::TBitMask<32> mask;
            mask.SetBit(1);
            mask.SetBit(9);
            mask.SetBit(31);
            Output("Bits are set", mask.CheckBit(1) && mask.CheckBit(9) && mask.CheckBit(31));
        }
    }

    void TestPerformance ()
    {
    
    }
} // namespace

void BitsTest (ETestType testType)
{
    switch (testType)
    {
    case TEST_TYPE_ALL:         for (unsigned i = 0; i < TEST_TYPES; ++i) if (i != testType) BitsTest(ETestType(i)); return;
    case TEST_TYPE_CORRECTNESS: return TestCorrectness();
    case TEST_TYPE_PERFORMANCE: return TestPerformance();
    }
}