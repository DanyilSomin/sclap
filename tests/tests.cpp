#include "gtest/gtest.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    getchar();
    return 0;
}

TEST(blaTest, test1)
{
    EXPECT_EQ (1,  1);
}