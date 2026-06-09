#pragma once

namespace common {

enum class Error {
    Ok,
    InvalidArgument,
    NotReady,
    NotFound,
    Timeout,
    Failed,
};

template <typename T>
class Result {
public:
    static Result ok(T value) {
        return Result(value, Error::Ok);
    }

    static Result err(Error error) {
        return Result(T {}, error);
    }

    bool is_ok() const {
        return error_ == Error::Ok;
    }

    explicit operator bool() const {
        return is_ok();
    }

    const T& value() const {
        return value_;
    }

    Error error() const {
        return error_;
    }

private:
    Result(T value, Error error) : value_(value), error_(error) {}

    T value_ {};
    Error error_ = Error::Ok;
};

}  // namespace common
