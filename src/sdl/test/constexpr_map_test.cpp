#include <gtest/gtest.h>
#include <constexpr_map.h>
#include <string_view>

namespace sdl {

// Test basic constexpr map functionality
constexpr auto testBasicMapCreation() -> bool {
    constexpr auto data = std::array{
        std::pair{1, 10},
        std::pair{2, 20},
        std::pair{3, 30}
    };
    constexpr auto map = Map(data);
    
    return map[1] == 10 && map[2] == 20 && map[3] == 30;
}

// Test map with different types
constexpr auto testDifferentTypes() -> bool {
    constexpr auto data = std::array{
        std::pair{std::string_view("hello"), 42},
        std::pair{std::string_view("world"), 99}
    };
    constexpr auto map = Map(data);
    
    return map[std::string_view("hello")] == 42 && 
           map[std::string_view("world")] == 99;
}

// Test makeMap helper function
constexpr auto testMakeMapHelper() -> bool {
    constexpr auto map = makeMap<int, int>(
        std::pair{10, 100},
        std::pair{20, 200}
    );
    
    return map[10] == 100 && map[20] == 200;
}

// Test makeConstexprMap function
constexpr auto testMakeConstexprMapHelper() -> bool {
    constexpr auto data = std::array{
        std::pair{5, 50},
        std::pair{6, 60}
    };
    constexpr auto map = makeConstexprMap(data);
    
    return map[5] == 50 && map[6] == 60;
}

// Test initializer list constructor
constexpr auto testInitializerListConstructor() -> bool {
    constexpr Map<int, int, 2> map{{
        {100, 1000},
        {200, 2000}
    }};
    
    return map[100] == 1000 && map[200] == 2000;
}

// Test single element map
constexpr auto testSingleElementMap() -> bool {
    constexpr auto data = std::array{
        std::pair{42, 84}
    };
    constexpr auto map = Map(data);
    
    return map[42] == 84;
}

// Test enum as key
enum class Color { Red, Green, Blue };

constexpr auto testEnumKeys() -> bool {
    constexpr auto data = std::array{
        std::pair{Color::Red, 255},
        std::pair{Color::Green, 128},
        std::pair{Color::Blue, 64}
    };
    constexpr auto map = Map(data);
    
    return map[Color::Red] == 255 && 
           map[Color::Green] == 128 && 
           map[Color::Blue] == 64;
}

// Test const char* keys
constexpr auto testConstCharKeys() -> bool {
    // Note: We can't use const char* directly in constexpr context easily
    // but we can test with other comparable types
    constexpr auto data = std::array{
        std::pair{1L, 'a'},
        std::pair{2L, 'b'}
    };
    constexpr auto map = Map(data);
    
    return map[1L] == 'a' && map[2L] == 'b';
}

// Compile-time tests using static_assert
static_assert(testBasicMapCreation());
static_assert(testDifferentTypes());
static_assert(testMakeMapHelper());
static_assert(testMakeConstexprMapHelper());
static_assert(testInitializerListConstructor());
static_assert(testSingleElementMap());
static_assert(testEnumKeys());
static_assert(testConstCharKeys());

// Runtime Google Test cases to ensure the functionality works at runtime too
TEST(ConstexprMapTest, BasicMapCreation) {
    constexpr auto data = std::array{
        std::pair{1, 10},
        std::pair{2, 20},
        std::pair{3, 30}
    };
    constexpr auto map = Map(data);
    
    EXPECT_EQ(map[1], 10);
    EXPECT_EQ(map[2], 20);
    EXPECT_EQ(map[3], 30);
}

TEST(ConstexprMapTest, DifferentTypes) {
    constexpr auto data = std::array{
        std::pair{std::string_view("hello"), 42},
        std::pair{std::string_view("world"), 99}
    };
    constexpr auto map = Map(data);
    
    EXPECT_EQ(map[std::string_view("hello")], 42);
    EXPECT_EQ(map[std::string_view("world")], 99);
}

TEST(ConstexprMapTest, MakeMapHelper) {
    constexpr auto map = makeMap<int, int>(
        std::pair{10, 100},
        std::pair{20, 200}
    );
    
    EXPECT_EQ(map[10], 100);
    EXPECT_EQ(map[20], 200);
}

TEST(ConstexprMapTest, MakeConstexprMapHelper) {
    constexpr auto data = std::array{
        std::pair{5, 50},
        std::pair{6, 60}
    };
    constexpr auto map = makeConstexprMap(data);
    
    EXPECT_EQ(map[5], 50);
    EXPECT_EQ(map[6], 60);
}

TEST(ConstexprMapTest, InitializerListConstructor) {
    constexpr Map<int, int, 2> map{{
        {100, 1000},
        {200, 2000}
    }};
    
    EXPECT_EQ(map[100], 1000);
    EXPECT_EQ(map[200], 2000);
}

TEST(ConstexprMapTest, SingleElementMap) {
    constexpr auto data = std::array{
        std::pair{42, 84}
    };
    constexpr auto map = Map(data);
    
    EXPECT_EQ(map[42], 84);
}

TEST(ConstexprMapTest, EnumKeys) {
    constexpr auto data = std::array{
        std::pair{Color::Red, 255},
        std::pair{Color::Green, 128},
        std::pair{Color::Blue, 64}
    };
    constexpr auto map = Map(data);
    
    EXPECT_EQ(map[Color::Red], 255);
    EXPECT_EQ(map[Color::Green], 128);
    EXPECT_EQ(map[Color::Blue], 64);
}

TEST(ConstexprMapTest, RuntimeKeyNotFoundThrows) {
    constexpr auto data = std::array{
        std::pair{1, 10},
        std::pair{2, 20}
    };
    constexpr auto map = Map(data);
    
    // This test must be done at runtime since exceptions can't be thrown in constexpr
    EXPECT_THROW(map[999], std::range_error);
}

TEST(ConstexprMapTest, MoveSemantics) {
    auto data = std::array{
        std::pair{1, 10},
        std::pair{2, 20}
    };
    auto map = Map(std::move(data));
    
    EXPECT_EQ(map[1], 10);
    EXPECT_EQ(map[2], 20);
}

} // namespace sdl