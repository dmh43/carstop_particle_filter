#include <limits.h>
#include <math.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../src/pf.h"
namespace {

    bool eq(float* vec1, float* vec2, int length) {
        bool acc = true;
        for (int i = 0; i < length; i++) {
            acc &= (vec1[i] == vec2[i]);
        }
        return acc;
    }

    TEST(InnerProductTest, NormSquared) {
        float vec1[2] = {1.0f, 2.0f};
        float vec2[2] = {1.0f, 2.0f};
        EXPECT_EQ(5, inner_product(vec1, vec2, 2));
    }

    TEST(InnerProductTest, Any) {
        float vec1[2] = {3.0f, 5.0f};
        float vec2[2] = {1.0f, 2.0f};
        EXPECT_EQ(13, inner_product(vec1, vec2, 2));
    }

    TEST(InnerProductTest, Orthogonal) {
        float vec1[2] = {-1.0f, 1.0f};
        float vec2[2] = {1.0f, 1.0f};
        EXPECT_EQ(0, inner_product(vec1, vec2, 2));
    }


    TEST(VecSubtractTest, Any) {
        float vec1[2] = {-1.0f, 1.0f};
        float vec2[2] = {1.0f, 1.0f};
        float result[2] = {-2.0, 0.0};
        EXPECT_TRUE(eq(vec_subtract(vec1, vec2, 2), result, 2));
    }


    TEST(MatVecMul, Identity) {
        float vec[] = {-1.0f, 1.0f};
        float mat[] = {1.0f, 0.0f, 0.0f, 1.0f};
        EXPECT_TRUE(eq(mat_vec_mul(mat, vec, 2, 2), vec, 2));
    }

    TEST(MatVecMul, Ones) {
        float vec[] = {1.0f, 1.0f};
        float mat[] = {1.0f, 1.0f, 1.0f, 1.0f};
        float result[] = {2.0f, 2.0f};
        EXPECT_TRUE(eq(mat_vec_mul(mat, vec, 2, 2), result, 2));
    }


    TEST(CalcNormSquaredIn, Ones) {
        float vec[] = {1.0f, 1.0f};
        float mat[] = {1.0f, 1.0f, 1.0f, 1.0f};
        EXPECT_EQ(calc_norm_squared_in(vec, mat, 2, 2), 4);
    }


    TEST(CalcUnnormalizedImportanceWeight, White) {
        float error[] = {1.0f, 1.0f};
        float cov[] = {1.0f, 0.0f, 0.0f, 1.0f};
        EXPECT_FLOAT_EQ(calc_unnormalized_importance_weight(error, cov), M_E);

        float no_error[] = {0.0f, 0.0f};
        EXPECT_FLOAT_EQ(calc_unnormalized_importance_weight(no_error, cov), 1);
    }


}
