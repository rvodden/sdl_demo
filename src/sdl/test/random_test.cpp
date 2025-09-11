#include <gtest/gtest.h>
#include <type_traits>

#include <sdl/random.h>

// Test constants
constexpr uint64_t kTestSeed = 12345;
constexpr int32_t kIntMin = 10;
constexpr int32_t kIntMax = 100;
constexpr float kFloatMin = 1.0f;
constexpr float kFloatMax = 10.0f;

// Tests for Random<int32_t>
class IntRandomTest : public ::testing::Test {
protected:
    void SetUp() override {
        random_ = std::make_unique<sdl::Random<int32_t>>(kTestSeed);
    }

    std::unique_ptr<sdl::Random<int32_t>> random_;
};

TEST_F(IntRandomTest, Constructor) {
    EXPECT_NO_THROW(sdl::Random<int32_t> r(kTestSeed));
    EXPECT_NO_THROW(sdl::Random<int32_t> r);  // Default seed
}

TEST_F(IntRandomTest, CopyConstructorDeleted) {
    // Verify that copy constructor is deleted
    static_assert(!std::is_copy_constructible_v<sdl::Random<int32_t>>);
}

TEST_F(IntRandomTest, MoveConstructorDeleted) {
    // Verify that move constructor is deleted
    static_assert(!std::is_move_constructible_v<sdl::Random<int32_t>>);
}

TEST_F(IntRandomTest, RangeValidBounds) {
    for (int i = 0; i < 100; ++i) {
        int32_t value = random_->range(kIntMin, kIntMax);
        EXPECT_GE(value, kIntMin);
        EXPECT_LT(value, kIntMax);
    }
}

TEST_F(IntRandomTest, RangeEqualBounds) {
    int32_t value = random_->range(kIntMin, kIntMin);
    EXPECT_EQ(value, kIntMin);
}

TEST_F(IntRandomTest, RangeInvalidBounds) {
    int32_t value = random_->range(kIntMax, kIntMin);  // max < min
    EXPECT_EQ(value, kIntMax);  // Should return min (which is max in this case)
}

// Tests for Random<float>
class FloatRandomTest : public ::testing::Test {
protected:
    void SetUp() override {
        random_ = std::make_unique<sdl::Random<float>>(kTestSeed);
    }

    std::unique_ptr<sdl::Random<float>> random_;
};

TEST_F(FloatRandomTest, Constructor) {
    EXPECT_NO_THROW(sdl::Random<float> r(kTestSeed));
    EXPECT_NO_THROW(sdl::Random<float> r);  // Default seed
}

TEST_F(FloatRandomTest, CopyConstructorDeleted) {
    // Verify that copy constructor is deleted
    static_assert(!std::is_copy_constructible_v<sdl::Random<float>>);
}

TEST_F(FloatRandomTest, RangeValidBounds) {
    for (int i = 0; i < 100; ++i) {
        float value = random_->range(kFloatMin, kFloatMax);
        EXPECT_GE(value, kFloatMin);
        EXPECT_LT(value, kFloatMax);
    }
}

TEST_F(FloatRandomTest, RangeEqualBounds) {
    float value = random_->range(kFloatMin, kFloatMin);
    EXPECT_FLOAT_EQ(value, kFloatMin);
}

TEST_F(FloatRandomTest, RangeInvalidBounds) {
    float value = random_->range(kFloatMax, kFloatMin);  // max < min
    EXPECT_FLOAT_EQ(value, kFloatMax);  // Should return min (which is max in this case)
}

// Tests for type aliases
TEST(RandomAliasTest, IntRandom) {
    EXPECT_NO_THROW(sdl::IntRandom r(kTestSeed));
    sdl::IntRandom r(kTestSeed);
    int32_t value = r.range(0, 100);
    EXPECT_GE(value, 0);
    EXPECT_LT(value, 100);
}

TEST(RandomAliasTest, FloatRandom) {
    EXPECT_NO_THROW(sdl::FloatRandom r(kTestSeed));
    sdl::FloatRandom r(kTestSeed);
    float value = r.range(0.0f, 1.0f);
    EXPECT_GE(value, 0.0f);
    EXPECT_LT(value, 1.0f);
}

// Tests for global random functions
TEST(GlobalRandomTest, Srand) {
    EXPECT_NO_THROW(sdl::random::srand(kTestSeed));
    EXPECT_NO_THROW(sdl::random::srand());  // Default seed
}

TEST(GlobalRandomTest, Rand) {
    sdl::random::srand(kTestSeed);
    for (int i = 0; i < 100; ++i) {
        int32_t value = sdl::random::rand(100);
        EXPECT_GE(value, 0);
        EXPECT_LT(value, 100);
    }
}

TEST(GlobalRandomTest, Randf) {
    sdl::random::srand(kTestSeed);
    for (int i = 0; i < 100; ++i) {
        float value = sdl::random::randf();
        EXPECT_GE(value, 0.0f);
        EXPECT_LT(value, 1.0f);
    }
}

TEST(GlobalRandomTest, RandBits) {
    sdl::random::srand(kTestSeed);
    for (int i = 0; i < 100; ++i) {
        [[maybe_unused]] uint32_t value = sdl::random::randBits();
        // Just ensure it returns some value (any uint32_t is valid)
        EXPECT_TRUE(true);  // If we get here, the function worked
    }
}

// Test reproducibility
TEST(RandomReproducibilityTest, SameSeedSameSequence) {
    constexpr uint64_t seed = 54321;
    sdl::Random<int32_t> r1(seed);
    sdl::Random<int32_t> r2(seed);
    
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(r1.range(0, 1000), r2.range(0, 1000));
    }
}

TEST(RandomReproducibilityTest, DifferentSeedDifferentSequence) {
    sdl::Random<int32_t> r1(12345);
    sdl::Random<int32_t> r2(54321);
    
    // With different seeds, sequences should be different
    // (Very small chance they could be the same, but extremely unlikely)
    bool different = false;
    for (int i = 0; i < 10; ++i) {
        if (r1.range(0, 1000) != r2.range(0, 1000)) {
            different = true;
            break;
        }
    }
    EXPECT_TRUE(different);
}