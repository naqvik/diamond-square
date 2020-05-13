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

class DiamondSquare {
private:
    int const size_;
protected:
    int const MAXDIM;  // maximum index value (array dimension - 1)
private:
    std::unique_ptr<uint8_t[]> arr_;
public:
    explicit DiamondSquare(int sz)
        : size_(sz),
          MAXDIM(sz-1),
          arr_{std::make_unique<uint8_t[]>(size_*size_)} {
        if ( !DiamondSquare::isValidArraySize(size_) )
            throw InvalidArraySize();
    }

    // allow array[r][c] notation in client
    uint8_t *operator[](int row) { return row * size_ + arr_.get(); }

    // allow array(r,c) notation in client
    virtual uint8_t &operator()(int row, int col) {
        return arr_[row * size_ + col];
    }
    int size() const { return size_; }

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
    static bool isValidArraySize(unsigned size) {
        // to be valid, size must be 2**n+1 for n=1,2,3,...

        // empty array and 1x1 array are invalid for our purposes
        if (size <= 1)
            return false;
    
        size = size - 1;
        return isPowerOfTwo(size);
    }

    virtual void diamond_phase_with_stepsize(int stepsize) {

    }
    virtual void square_phase_with_stepsize(int stepsize) {
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

    unsigned calc_average(std::vector<std::pair<int,int>> const& coords) {
        for (auto p: coords) {
            access_pattern += std::to_string(p.first) +
                std::to_string(p.second) + " ";
        }

        return 0u;
    }
    std::vector<std::pair<int,int>>
    make_diamond_neighbour_list(int r, int c, int offset) {
        // list of row,col coordinates
        std::vector<std::pair<int,int>> coords;

        // find the centre
        int rc = r+offset;
        int cc = c+offset;

        // calculate all diamond neighbour source cell coords here
        // NW
        if (rc-offset >=0 && cc-offset >= 0) {
            coords.push_back({rc-offset, cc-offset});
        }
        // NE
        if (rc-offset >= 0 && cc+offset <= MAXDIM) {
            coords.push_back({rc-offset, cc+offset});
        }
        // SW
        if (rc+offset <= MAXDIM && cc-offset >= 0) {
            coords.push_back({rc+offset, cc-offset});
        }
        // SE
        if (rc+offset <= MAXDIM && cc+offset <= MAXDIM) {
            coords.push_back({rc+offset, cc+offset});
        }
        return coords;
    }
    void update_cell(int r, int c, unsigned value) {
        if (value > 0xffu)
            value = 0xffu; // saturate
        (*this)(r,c) = value;
        
        access_pattern += "->"s;
        access_pattern += std::to_string(r) +
            std::to_string(c) + "\n";
    }
    virtual void diamond_phase_with_stepsize(int stepsize) override {
        access_pattern += "diamond:stepsize:" +
            std::to_string(stepsize) + "\n";

        access_pattern += "read:";
        int offset = stepsize/2;  // how far away are neighbours?

        for (int r=0; r < MAXDIM; r += stepsize) {
            for (int c=0; c < MAXDIM; c += stepsize) {
                auto coords = make_diamond_neighbour_list(r,c,offset);

                // calculate average, store in destination
                unsigned value = calc_average(coords);

                update_cell(r+offset, c+offset, value);
            }
        }

        DiamondSquare::diamond_phase_with_stepsize(stepsize);
    }
    virtual void square_phase_with_stepsize(int stepsize) override {
        access_pattern += "square:" +
            std::to_string(stepsize) + "\n";
        DiamondSquare::square_phase_with_stepsize(stepsize);
    }
};
