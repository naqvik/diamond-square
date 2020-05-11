// -*- c++ -*-

#include <cstdint>
#include <iostream>

#include "../src/ds.h"          // implementation

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE( "Can successfully create 3x3 array" ) {
    arr = Array(3);
    REQUIRE( arr.size() == 3 );
}
