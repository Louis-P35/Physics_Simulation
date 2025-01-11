#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();

	int result = RUN_ALL_TESTS();

	std::cin.get();

	return result;
}
