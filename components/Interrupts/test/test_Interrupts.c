#include "unity.h"
#include "Interrupts.h"

TEST_CASE("test_func should return true", "Interrupts")
{
    TEST_ASSERT_EQUAL(true, test_func1());
}
