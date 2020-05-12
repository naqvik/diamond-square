// -*- c++ -*-

#include <cstdint>
#include <iostream>
#include <string>

#include "../src/ds.h"          // implementation

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


/**
   Subclass of DiamondSquare, overrides the algorithm to log access pattern

   Rationale: the test code should instantiate this "spy" type instead
   of the parent type, so that it can verify that the diamond phase
   and the square phase are both accessing memory in the correct
   pattern.  For the diamond phase the updated square depends on the
   NW, NE, SW, SE neighbours, in that order.  For the square phase
   the updated square depends on the N, W, E, S neighbours, in that
   order.
 */
class DiamondSquareSpy : public DiamondSquare {
public:
    using DiamondSquare::DiamondSquare;

    std::string access_pattern = "";

    virtual void diamond_phase() override {
        access_pattern += "diamond:stepsize:" + std::to_string(stepsize());
        DiamondSquare::diamond_phase();
    }
    virtual void square_phase() override {
        access_pattern += "square:";
    }
    virtual uint8_t &operator()(int row, int col) override {
        access_pattern +=
            std::to_string(row) + std::to_string(col) + " ";
        return DiamondSquare::operator()(row,col);
    }

};

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
    std::string expected_access_pattern =
        "diamond:stepsize:2\n"
        "(00 02 20 22)->(11)\n";
    auto a = DiamondSquareSpy(3);
    a.diamond_phase();
    REQUIRE( startsWith(a.access_pattern, "diamond:stepsize:2" ) );
}
