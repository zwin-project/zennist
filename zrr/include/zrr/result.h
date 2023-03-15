#pragma once

#include "zrr/error.h"

namespace zrr {

template <typename T>
class Result
{
 public:
  // noncopyable
  Result(const Result &other) = delete;
  Result &operator=(const Result &other) = delete;

  Result() = delete;

  // NOLINTNEXTLINE
  Result(T &&value) : value_(value), error_(Error::empty()) {}

  // NOLINTNEXTLINE
  Result(Error &&error) : error_(std::move(error)) {}

  ~Result() = default;

  T *operator->() { return &value_; }
  const T *operator->() const { return &value_; }

  T &operator*() { return value_; }
  const T &operator*() const { return value_; }

  const Error &error() const { return error_; }

 private:
  T value_;

  const Error error_;
};

}  // namespace zrr
