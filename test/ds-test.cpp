// -*- c++ -*-

#include <cstdint>
#include <iostream>
//#include <string>

#include "../src/ds.h"          // implementation

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

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

// TEST_CASE( "3x3 1st diamond access pattern is correct" ) {
//     std::string expected_access_pattern =
//         "diamond:stepsize:2\n" +
//         "(00 02 20 22)->(11)\n";
//     a = DiamondSquare(3);
//     a.diamond_phase(3);
//     a.square_phase(3);

// }
