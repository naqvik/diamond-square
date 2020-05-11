// -*- c++ -*-

#include <cstdint>
#include <iostream>

#include "../src/ds.h"          // implementation

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE( "Can successfully create 3x3 array" ) {
    auto arr = Array(3);
    REQUIRE( arr.size() == 3 );

    SUBCASE( "Can write values and read them back" ) {
        arr[2][2] = 127;
        REQUIRE( arr[2][2] == 127 );
    }
}

TEST_CASE( "Must only create arrays of size 2**n+1, where n=1,2,3,..." ) {
    // Need to detect invalid array sizes, when constructing
    CHECK( isValidArraySize(3) == true  );  // 3 == 2**1+1
    CHECK( isValidArraySize(8) == false );  // invalid
    CHECK( isValidArraySize(17)== true );   // 17 = 2**4+1
}
