#include "gtest/gtest.h"

class BaseTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

    static void SetUpTestSuite() {}

    static void TearDownTestSuite() {}

protected:
};

TEST_F(BaseTest, World) {
    ASSERT_EQ(1, 1);
}
