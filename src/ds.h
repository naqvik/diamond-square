// -*- c++ -*-
// Diamond-Square Implementation
#include <iostream>
#include <cstdint>
#include <utility>              // for std::pair
#include <vector>
#include <memory>
#include <string>

struct InvalidArraySize {
};

using element_type = uint8_t;

using Coords = std::vector<std::pair<int,int>>;
struct InitialCornerValues {
    element_type NW, NE, SW, SE;
};

class DiamondSquare {
private:
    int const size_;
public:
    int const MAXDIM;  // maximum index value (array dimension - 1)
private:
    std::unique_ptr<element_type[]> arr_;
public:
    explicit DiamondSquare(int sz)
        : size_(sz),
          MAXDIM(sz-1),
          arr_{std::make_unique<element_type[]>(size_*size_)} {
        if ( !DiamondSquare::isValidArraySize(size_) )
            throw InvalidArraySize();
    }
    explicit DiamondSquare(int sz, InitialCornerValues corners)
        : size_(sz),
          MAXDIM(sz-1),
          arr_{std::make_unique<element_type[]>(size_*size_)} {
        if ( !DiamondSquare::isValidArraySize(size_) )
            throw InvalidArraySize();
        (*this)[0][0] = corners.NW;
        (*this)[0][MAXDIM] = corners.NE;
        (*this)[MAXDIM][0] = corners.SW;
        (*this)[MAXDIM][MAXDIM] = corners.SE;
    }

    // allow array[r][c] notation in client
    element_type *operator[](int row) { return row * size_ + arr_.get(); }

    // allow array(r,c) notation in client
    virtual element_type &operator()(int row, int col) {
        return arr_[row * size_ + col];
    }
    int size() const { return size_; }

    virtual void update_cell(int r, int c, unsigned value) {
        if (value > 0xffu)
            value = 0xffu; // saturate
        (*this)(r,c) = value;
    }
    void interpolate() {
        int stepsize = size() - 1;
        for (stepsize = size() - 1; stepsize >= 2; stepsize/=2) {
            //std::cout << "i:stepsize:" << stepsize << std::endl;
            diamond_phase_with_stepsize(stepsize);
            square_phase_with_stepsize(stepsize);
        }
    }
private:
    /** @pre n >= 2 */
    static bool isPowerOfTwo(unsigned n) {
        // Clever code: n&(n-1) will be zero IFF n had exactly one 1 bit.
        // Which is the same as saying it's a power of two.
        // example: n = 00001000
        //        n-1 = 00000111
        //  --------------------
        //  n & (n-1) = 00000000
        if ( (n & (n-1)) == 0 ) {
            return true;
        }
        return false;
    }
public:
    Coords make_diamond_neighbour_list(int r, int c, int offset) {
        // list of row,col coordinates
        Coords coords;

        // calculate all diamond neighbour source cell coords here
        if (r-offset >=0 && c-offset >= 0)
            coords.push_back({r-offset, c-offset}); // NW
        if (r-offset >= 0 && c+offset <= MAXDIM)
            coords.push_back({r-offset, c+offset}); // NE
        if (r+offset <= MAXDIM && c-offset >= 0)
            coords.push_back({r+offset, c-offset}); // SW
        if (r+offset <= MAXDIM && c+offset <= MAXDIM)
            coords.push_back({r+offset, c+offset}); // SE

        return coords;
    }
    Coords make_square_neighbour_list(int r, int c, int offset) {
        // list of row,col coordinates
        Coords coords;

        if (r-offset >= 0)
            coords.push_back({r-offset,c}); // North
        if (c-offset >= 0)
            coords.push_back({r, c-offset}); // West
        if (c+offset <= MAXDIM)
            coords.push_back({r, c+offset}); // East
        if (r+offset <= MAXDIM)
            coords.push_back({r+offset, c}); // South

        return coords;
    }

    virtual unsigned calc_average(Coords const& coords) {
        unsigned sum = 0;
        for (auto coord: coords)
            sum += (*this)(coord.first, coord.second);
        return sum / coords.size();
    }

    static bool isValidArraySize(unsigned size) {
        // to be valid, size must be 2**n+1 for n=1,2,3,...

        // empty array and 1x1 array are invalid for our purposes
        if (size <= 1)
            return false;
    
        size = size - 1;
        return isPowerOfTwo(size);
    }

    virtual void diamond_phase_with_stepsize(int stepsize) {
        int offset = stepsize/2;  // how far away are neighbours?

        for (int r=0; r < MAXDIM; r += stepsize) {
            for (int c=0; c < MAXDIM; c += stepsize) {
                auto coords = make_diamond_neighbour_list(
                    r+offset, c+offset, offset);

                // calculate average, store in destination
                unsigned value = calc_average(coords);

                update_cell(r+offset, c+offset, value);
            }
        }
    }
    virtual void square_phase_with_stepsize(int stepsize) {
        int offset = stepsize/2;  // how far away are neighbours?

        int row_parity = 0; // 0=even, 1=odd

        for (int r=0; r <= MAXDIM; r += stepsize/2, row_parity ^= 1) {
            // on even parity rows, offset the columns
            for (int c = row_parity==0 ? offset : 0;
                 c <= MAXDIM; c += stepsize) {
                //std::cout << "r,c:" << r << "," << c << "\n";
                auto coords = make_square_neighbour_list(r,c,offset);

                // calculate average, store in destination
                unsigned value = calc_average(coords);

                update_cell(r, c, value);
            }
        }
    }
};


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

using namespace std::literals::string_literals;

class DiamondSquareSpy : public DiamondSquare {
public:
    using DiamondSquare::DiamondSquare;

    std::string access_pattern = "";

    unsigned calc_average(Coords const& coords) override {
        for (auto p: coords) {
            access_pattern += std::to_string(p.first) +
                std::to_string(p.second) + " ";
        }

        return DiamondSquare::calc_average(coords);
    }
    virtual void update_cell(int r, int c, unsigned /*value*/) override {
        // DiamondSquare::update_cell(r,c,value);
        
        access_pattern += "->"s;
        access_pattern += std::to_string(r) +
            std::to_string(c) + "\n";
    }
    virtual void diamond_phase_with_stepsize(int stepsize) override {
        access_pattern += "diamond:stepsize:" +
            std::to_string(stepsize) + "\n";

        access_pattern += "read:";
        DiamondSquare::diamond_phase_with_stepsize(stepsize);
    }
    virtual void square_phase_with_stepsize(int stepsize) override {
        access_pattern += "square:stepsize:" +
            std::to_string(stepsize) + "\n";

        access_pattern += "read:";
        DiamondSquare::square_phase_with_stepsize(stepsize);
    }
};
