// -*- c++ -*-
// Diamond-Square Implementation
#include <cstdint>
#include <memory>


class Array {
private:
    int const size_;
    std::unique_ptr<uint8_t[]> arr_;
public:
    explicit Array(int sz)
        : size_(sz),
          arr_{std::make_unique<uint8_t[]>(size_*size_)} {}

    // allow array[r][c] notation in client
    uint8_t *operator[](int row) { return row * size_ + arr_.get(); };

    // allow array(r,c) notation in client
    uint8_t &operator()(int row, int col) {
        return arr_[row * size_ + col];
    };
    int size() const { return size_; }
};

inline bool isPowerOfTwo(unsigned n) {
    if (n <= 1) return false;  // empty array and 1x1 array are
                               // invalid for our purposes

    // Clever code: will be zero IFF n had a single 1 bit. An 8-bit
    // example: n = 00001000
    //        n-1 = 00000111
    //  n & (n-1) = 00000000
    if ( (n & (n-1)) == 0 ) {
        return true;
    }
    return false;
}
