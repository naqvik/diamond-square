// -*- c++ -*-

#include <cstdint>
#include <iostream>
#include <string>

#include "../src/ds.h"          // implementation

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

TEST_CASE( "create 3x3 spy for diamond access pattern" ) {
    using namespace std::literals::string_literals;

    auto a = DiamondSquareSpy(3);
    a.diamond_phase_with_stepsize(2);
    REQUIRE(
        startsWith(a.access_pattern,
                   "diamond:stepsize:2\n"s +
                   "update:11")
        );
}
