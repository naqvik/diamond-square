// -*- c++ -*-
// Diamond-Square Implementation
#include <iostream>
#include <cstdint>
#include <memory>
#include <string>

struct InvalidArraySize {
};

class DiamondSquare {
private:
    int const size_;
    int const step_size_;
    std::unique_ptr<uint8_t[]> arr_;
public:
    explicit DiamondSquare(int sz)
        : size_(sz),
          step_size_(sz-1),
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
    int stepsize() const { return step_size_; }

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
class DiamondSquareSpy : public DiamondSquare {
public:
    using DiamondSquare::DiamondSquare;

    std::string access_pattern = "";

    virtual void diamond_phase_with_stepsize(int stepsize) override {
        access_pattern += "diamond:stepsize:" +
            std::to_string(stepsize) + "\n";
        DiamondSquare::diamond_phase_with_stepsize(stepsize);
    }
    virtual void square_phase_with_stepsize(int stepsize) override {
        access_pattern += "square:" +
            std::to_string(stepsize) + "\n";
        DiamondSquare::square_phase_with_stepsize(stepsize);
    }
};
