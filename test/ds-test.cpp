// -*- c++ -*-

#include <cstdint>
#include <iostream>
#include <string>

#include "../src/ds.h"          // implementation

#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
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
        INFO( "ACCESS PATTERN:" << a.access_pattern );
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
        INFO( "ACCESS PATTERN:" << a.access_pattern );
        REQUIRE( startsWith(
                     a.access_pattern,
                     "diamond:stepsize:4\nread:00 04 40 44 ->22"s) );
    }
    SUBCASE( "5x5 di:stepsize:2 reads and updates correct cells" ) {
        a.diamond_phase_with_stepsize(2);
        INFO("ACCESS PATTERN:" << a.access_pattern);
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
    INFO( "ACCESS PATTERN:" << a.access_pattern );
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

TEST_CASE( "3x3 complete numerical example w/o randomness: " ) {
    auto a = DiamondSquare(3, {14,64,64,110});

    SUBCASE( "di:step:2 corners 14 64 64 110 ->average 63" ) {
        a.diamond_phase_with_stepsize(2);
        REQUIRE( a(1,1) == 63 );
        /*  0   1   2
          +---+---+---+
        0 | 14| 0 | 64|
          +---+---+---+
        1 | 0 | 63| 0 |
          +---+---+---+
        2 | 64| 0 |110|
          +---+---+---+
         */

        SUBCASE( "sq:step:2" ) {
            a.square_phase_with_stepsize(2);
            REQUIRE( a(0,1) == 47 );
            REQUIRE( a(1,0) == 47 );
            REQUIRE( a(1,2) == 79 );
            REQUIRE( a(2,1) == 79 );
            /*   0   1   2
               +---+---+---+
             0 | 14| 47| 64|
               +---+---+---+
             1 | 47| 63| 79|
               +---+---+---+
             2 | 64| 79|110|
               +---+---+---+
            */
        }
    }
    SUBCASE( "Implement 3x3 interpolation" ) {
        a.interpolate();
        REQUIRE( a(1,1) == 63 );
        REQUIRE( a(0,1) == 47 );
        REQUIRE( a(1,0) == 47 );
        REQUIRE( a(1,2) == 79 );
        REQUIRE( a(2,1) == 79 );
    }
}

template <int sz>
void check_array(DiamondSquare & actual, element_type const expected[sz][sz]) {
    for (int i=0; i<sz; i+=1)
        for (int j=0; j<sz; j+=1) {
            INFO( "(i,j)=(" << i <<","<<j<<")" );
            CHECK( actual(i,j) == doctest::Approx(expected[i][j]) );
        }
}

TEST_CASE( "5x5 complete numerical example, w/o randomness" ) {
    auto a = DiamondSquare(5, {14,64,204,110});
    a.interpolate();

    element_type expected[5][5] = {
        { 14, 46, 58, 66, 64},
        { 62, 68, 75, 77, 77},
        {105,101, 98, 93, 90},
        {148,136,119,108,102},
        {204,159,137,118,110},
    };
    check_array<5>(a, expected);
}

TEST_CASE( "9x9 init with {1,1,1,1} leads to all 1's" ) {
    auto a = DiamondSquare(9, {1,1,1,1});
    a.interpolate();
    for (int i=0; i < a.size(); ++i)
        for (int j=0; j < a.size(); ++j) {
            INFO( "a(" << i << ","<< j << ") =" << a(i,j) );
            REQUIRE(a(i,j) == 1);
        }
}

TEST_CASE( "9x9 complete numerical example: w/o randomness" ) {
    auto a = DiamondSquare(9, {9, 67, 225, 191} );
    a.interpolate();

    // double 9x9 array created with spreadsheet
    double expected_temp[9][9] = {
        {9, 37.6018518518518, 51.5555555555556, 63.3252314814815,
         66.3333333333333, 75.0474537037037, 76.3888888888889,
         75.7824074074074, 67},
        {43.4537037037037, 52.25, 63.8064236111111, 72.0868055555555,
         77.9913194444444, 82.4201388888889, 84.6501736111111, 83.9583333333333,
         82.5231481481481},
        {69.1111111111111, 74.4357638888889, 79.3333333333333, 86.5243055555555,
         91.125, 94.5451388888889, 95.8333333333333, 96.8940972222222,
         96.6111111111111},
        {95.0532407407407, 97.0486111111111, 100.171006944444, 103.552083333333,
         106.619791666667, 108.802083333333, 110.264756944444, 111.173611111111,
         111.594907407407},
        {119, 119.822916666667, 120.75, 121.84375, 123, 124.15625, 125.25,
         126.177083333333, 127},
        {143.349537037037, 142.493055555556, 141.245659722222, 140.072916666667,
         139.380208333333, 139.572916666667, 140.318576388889, 141.284722222222,
         142.002314814815},
        {168.555555555556, 164.678819444444, 161.666666666667, 156.965277777778,
         154.875, 153.965277777778, 155.166666666667, 155.991319444444,
         157.722222222222},
        {193.185185185185, 186, 176.922743055556, 171.246527777778,
         168.008680555556, 166.246527777778, 166.620659722222, 169.791666666667,
         172.837962962963},
        {225, 199.925925925926, 188.777777777778, 179.896990740741,
         179.666666666667, 173.730324074074, 175.277777777778, 178.689814814815,
         191},
    };


// //     // currently my implementation uses cell values of uint8_t, so
// //     // this is the above 9x9 grid with the values truncated to int.
// //     // I've carefully chosen the initial values so nothing exceeds 255
// //     // in value.
// //     element_type expected[9][9] = {
// //         {9, 28, 37, 45, 49, 58, 63, 67, 67},
// //         {32, 37, 45, 50, 55, 61, 67, 73, 76},
// //         {50, 52, 54, 60, 60, 70, 73, 82, 89},
// //         {69, 67, 69, 78, 86, 86, 85, 94, 103},
// //         {89, 84, 76, 95, 123,	100, 87, 106, 127},
// //         {112, 105, 99, 102, 107, 105, 106, 118, 129},
// //         {142, 130, 114, 110, 97, 110, 115, 131, 144},
// //         {175, 160, 139, 126, 120, 123, 133, 149, 161},
// //         {225, 181, 158, 139, 134, 135, 146, 162, 191},
// //     };

    check_array<9>(a, expected_temp);
}
