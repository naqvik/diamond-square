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
