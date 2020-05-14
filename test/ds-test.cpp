// -*- c++ -*-

#include <cstdint>
#include <iostream>
#include <string>

#include "../src/ds.h"          // implementation

//#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

bool startsWith(const std::string& str, const std::string& pattern) {
    return pattern.length() <= str.length() &&
        std::equal(pattern.begin(), pattern.end(), str.begin());
}

TEST_CASE( "Can successfully create 3x3 array" ) {
    auto arr = DiamondSquare(3);
    REQUIRE( arr.size() == 3 );

    SUBCASE( "Can write values and read them back" ) {
        arr[2][2] = 127;
        REQUIRE( arr[2][2] == 127 );
    }
}

TEST_CASE( "Must only create arrays of size 2**n+1, where n=1,2,3,..." ) {
    
    // Need to detect invalid array sizes, when constructing
    CHECK( DiamondSquare::isValidArraySize(3) == true  );  // 3 == 2**1+1
    CHECK( DiamondSquare::isValidArraySize(8) == false );  // invalid
    CHECK( DiamondSquare::isValidArraySize(17)== true );   // 17 = 2**4+1

    SUBCASE( "Invalid array size: throws an exception" ) {
        REQUIRE_THROWS_AS( DiamondSquare(4), InvalidArraySize& );
    }
}

TEST_CASE( "3x3 spy for diamond access pattern" ) {
    using namespace std::literals::string_literals;

    auto a = DiamondSquareSpy(3);
    SUBCASE( "3x3 di:stepsize:2 reads and updates correct cells" ) {
        a.diamond_phase_with_stepsize(2);
        std::cout << "ACCESS PATTERN:" << a.access_pattern << "\n";
        REQUIRE( startsWith(
                     a.access_pattern,
                     "diamond:stepsize:2\nread:00 02 20 22 ->11"s) );
    }
}

TEST_CASE( "5x5 spy for diamond access pattern" ) {
    using namespace std::literals::string_literals;

    auto a = DiamondSquareSpy(5);
    SUBCASE( "5x5 di:stepsize:4 reads and updates correct cells" ) {
        a.diamond_phase_with_stepsize(4);
        std::cout << "ACCESS PATTERN:" << a.access_pattern << "\n";
        REQUIRE( startsWith(
                     a.access_pattern,
                     "diamond:stepsize:4\nread:00 04 40 44 ->22"s) );
    }
    SUBCASE( "5x5 di:stepsize:2 reads and updates correct cells" ) {
        a.diamond_phase_with_stepsize(2);
        std::cout << "ACCESS PATTERN:" << a.access_pattern << "\n";
        REQUIRE( startsWith(
                     a.access_pattern,
                     "diamond:stepsize:2\nread:00 02 20 22 ->11\n"
                     "02 04 22 24 ->13\n"
                     "20 22 40 42 ->31\n"
                     "22 24 42 44 ->33"
                     ) );
    }
}

TEST_CASE( "3x3 spy for square access pattern" ) {
    using namespace std::literals::string_literals;

    auto a = DiamondSquareSpy(3);
    a.square_phase_with_stepsize(2);
    std::cout << "ACCESS PATTERN:" << a.access_pattern << "\n";
    REQUIRE( startsWith(
                 a.access_pattern,
                 "square:stepsize:2\n"
                 "read:00 02 11 ->01\n"
                 "00 11 20 ->10\n"
                 "02 11 22 ->12\n"
                 "11 20 22 ->21"
                 ) );
}

TEST_CASE( "5x5 spy for square access pattern" ) {
    using namespace std::literals::string_literals;

    auto a = DiamondSquareSpy(5);
    SUBCASE( "square:stepsize:4" ) {
        a.square_phase_with_stepsize(4);
        INFO( "ACCESS PATTERN:"s + a.access_pattern );

        REQUIRE( startsWith(
                     a.access_pattern,
                     "square:stepsize:4\n"
                     "read:00 04 22 ->02\n"
                     "00 22 40 ->20\n"
                     "04 22 44 ->24\n"
                     "22 40 44 ->42"
                     ) );
    }
    SUBCASE( "square:stepsize:2" ) {
        a.square_phase_with_stepsize(2);
        INFO( "ACCESS PATTERN:"s + a.access_pattern );

        REQUIRE( startsWith(
                     a.access_pattern,
                     "square:stepsize:2\n"
                     "read:00 02 11 ->01\n"
                     "02 04 13 ->03\n"
                     "00 11 20 ->10\n"
                     ) );
    }
}
