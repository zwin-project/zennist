#pragma once

#include <memory>
#include <string>

namespace zrr {

class Error
{
 public:
  explicit Error(const char *format, ...) __attribute__((format(printf, 2, 3)));

  Error(Error cause, const char *format, ...)
      __attribute__((format(printf, 3, 4)));

  ~Error() = default;

  Error(const Error &other);

  Error &operator=(const Error &other);

  explicit operator bool() const;

  bool operator!() const;

  static Error empty();

  std::string to_string();

 private:
  Error();

  std::string Format(const char *format, va_list arg) const;

  std::unique_ptr<Error> make_cause_unique() const;

  bool has_error_;
  std::string reason_;
  std::unique_ptr<Error> cause_;
};

}  // namespace zrr
