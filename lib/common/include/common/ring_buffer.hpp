#pragma once

#include <array>
#include <cstddef>

namespace common {

template <typename T, size_t Capacity>
class RingBuffer {
public:
    static_assert(Capacity > 0, "RingBuffer capacity must be greater than zero");

    bool push(const T& value) {
        if (full()) {
            return false;
        }
        data_[write_] = value;
        write_ = (write_ + 1) % Capacity;
        ++size_;
        return true;
    }

    bool pop(T& value) {
        if (empty()) {
            return false;
        }
        value = data_[read_];
        read_ = (read_ + 1) % Capacity;
        --size_;
        return true;
    }

    bool empty() const {
        return size_ == 0;
    }

    bool full() const {
        return size_ == Capacity;
    }

    size_t size() const {
        return size_;
    }

    void clear() {
        read_ = 0;
        write_ = 0;
        size_ = 0;
    }

private:
    std::array<T, Capacity> data_ {};
    size_t read_ = 0;
    size_t write_ = 0;
    size_t size_ = 0;
};

}  // namespace common
