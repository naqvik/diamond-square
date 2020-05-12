// -*- c++ -*-
// Diamond-Square Implementation
#include <cstdint>
#include <memory>

struct InvalidArraySize {
};

class Array {
private:
    int const size_;
    std::unique_ptr<uint8_t[]> arr_;
public:
    explicit Array(int sz)
        : size_(sz),
          arr_{std::make_unique<uint8_t[]>(size_*size_)} {
        if ( !Array::isValidArraySize(size_) )
            throw InvalidArraySize();
    }

    // allow array[r][c] notation in client
    uint8_t *operator[](int row) { return row * size_ + arr_.get(); };

    // allow array(r,c) notation in client
    uint8_t &operator()(int row, int col) {
        return arr_[row * size_ + col];
    };
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
};


