#include <gtest/gtest.h>
#include "core/farm_os.h"

TEST(AddFunctionTest, addsTwoNumbers)
{
    EXPECT_EQ(farmos::add(2, 3), 5);
    EXPECT_EQ(farmos::add(-1, 1), 0);
}
TEST(AddFunctionTest, anotherEq)
{   
    EXPECT_EQ(farmos::add(0, 0), 0);
}
