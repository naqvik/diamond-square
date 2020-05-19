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
void check_array(DiamondSquare & actual, uint8_t const expected[5][5]) {
    for (int i=0; i<sz; i+=1)
        for (int j=0; j<sz; j+=1) {
            INFO( "(i,j)=(" << i <<","<<j<<")" );
            CHECK( actual(i,j) == expected[i][j] );
        }
}

TEST_CASE( "5x5 complete numerical example, w/o randomness" ) {
    auto a = DiamondSquare(5, {14,64,204,110});
    a.interpolate();

    uint8_t expected[5][5] = {
        { 14, 46, 58, 66, 64},
        { 62, 68, 75, 77, 77},
        {105,101, 98, 93, 90},
        {148,136,119,108,102},
        {204,159,137,118,110},
    };
    check_array<5>(a, expected);
}

// TEST_CASE( "9x9 complete numerical example: w/o randomness" ) {
//     auto a = DiamondSquare(9, {9, 67, 225, 191} );
//     a.interpolate();

//     // // Keep a double 9x9 array for future use
//     // double expected_temp[9][9] = {
//     //     {9, 28.1888888888889, 37.65, 45.9383333333333, 49.75,
//     //      58.2494444444444, 63.3666666666667, 67.8583333333333, 67},
//     //     {32.5777777777778, 37.9166666666667, 45.0454166666667, 50.415,
//     //      55.4641666666667, 61.6316666666667, 67.8891666666667, 73.2083333333333,
//     //      76.4416666666667},
//     //     {50.8166666666667, 52.635625, 54.2, 60.758125,
//     //      60.06, 70.2672916666667, 73.35, 82.4916666666667, 89.1166666666667},
//     //     {69.2252777777778, 67.6091666666667, 69.0841666666667, 78.3575,
//     //      86.86125, 86.0275, 85.3422916666667, 94.2916666666667, 103.469444444444},
//     //     {89.25, 84.67625, 76.17, 95.07125,
//     //      123,	100.63875, 87.7, 106.8875, 127},
//     //     {112.603055555556, 105.675833333333, 99.7508333333333, 102.7575,
//     //      107.26125, 105.8275, 106.808958333333, 118.558333333333, 129.980555555556},
//     //     {142.883333333333, 130.760625, 114.4, 110.195625,
//     //      97.46, 110.504791666667, 115.15, 131.866666666667, 144.383333333333},
//     //     {175.988888888889, 160.083333333333, 139.674583333333, 126.165,
//     //      120.489166666667, 123.581666666667, 133.768333333333, 149.375, 161.586111111111},
//     //     {225, 181.044444444444, 158.05, 139.655,
//     //      134.75, 135.099444444444, 146.966666666667, 162.447222222222, 191},
//     // };


//     // currently my implementation uses cell values of uint8_t, so
//     // this is the above 9x9 grid with the values truncated to int.
//     // I've carefully chosen the initial values so nothing exceeds 255
//     // in value.
//     element_type expected[9][9] = {
//         {9, 28, 37, 45, 49, 58, 63, 67, 67},
//         {32, 37, 45, 50, 55, 61, 67, 73, 76},
//         {50, 52, 54, 60, 60, 70, 73, 82, 89},
//         {69, 67, 69, 78, 86, 86, 85, 94, 103},
//         {89, 84, 76, 95, 123,	100, 87, 106, 127},
//         {112, 105, 99, 102, 107, 105, 106, 118, 129},
//         {142, 130, 114, 110, 97, 110, 115, 131, 144},
//         {175, 160, 139, 126, 120, 123, 133, 149, 161},
//         {225, 181, 158, 139, 134, 135, 146, 162, 191},
//     };

//     check_array<9>(a, expected);
// }
