#include <limits.h>
#include "gtest/gtest.h"

#include "../src/pf.h"
namespace {

    TEST(InnerProductTest, NormSquared) {
        float vec1[2] = {1.0f, 2.0f};
        float vec2[2] = {1.0f, 2.0f};

        EXPECT_EQ(5, inner_product(vec1, vec2, 2));
    }
}
