#include "01_bits.h"
#include "bits\bitmask.h"

namespace
{
    void TestCorrectness ()
    {
        // Set some bits and then check them.
        {
            Baseless::Bits::TBitMask<32> mask;
            mask.SetBit(1);
            mask.SetBit(9);
            mask.SetBit(31);
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
    case TEST_TYPE_CORRECTNESS: return TestCorrectness();
    case TEST_TYPE_PERFORMANCE: return TestPerformance();
    }
}