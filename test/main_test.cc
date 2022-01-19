#include <gtest/gtest.h>
using namespace std;

int main(int argc, char *argv[])
{
    //testing::GTEST_FLAG(output) = "xml:";
    testing::InitGoogleTest(&argc, argv);
    int n = RUN_ALL_TESTS();
    return 0;
}