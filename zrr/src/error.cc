#include "zrr/error.h"

#include <cstdarg>
#include <memory>
#include <vector>

namespace zrr {

Error::Error(const char *format, ...) : has_error_(true)
{
  va_list arg;
  va_start(arg, format);

  reason_ = Format(format, arg);

  va_end(arg);
}

Error::Error(Error cause, const char *format, ...)
    : has_error_(true), cause_(std::make_unique<Error>(std::move(cause)))
{
  va_list arg;
  va_start(arg, format);

  reason_ = Format(format, arg);

  va_end(arg);
}

Error::Error() : has_error_(false){};

Error::Error(const Error &other)
    : has_error_(other.has_error_),
      reason_(other.reason_),
      cause_(other.make_cause_unique())
{}

Error &
Error::operator=(const Error &other)
{
  if (this != &other) {
    cause_ = other.make_cause_unique();
    reason_ = other.reason_;
    has_error_ = other.has_error_;
  }
  return *this;
}

Error::operator bool() const { return has_error_; }

bool
Error::operator!() const
{
  return !static_cast<bool>(*this);
}

Error
Error::empty()
{
  return Error();
}

std::string
Error::to_string()
{
  std::string description = reason_;
  if (cause_ && *cause_) {
    description += "\n";
    description += "\tCaused by: " + cause_->to_string();
  }

  return description;
}

std::string
Error::Format(const char *format, va_list arg) const
{
  va_list arg_copy;

  va_copy(arg_copy, arg);
  size_t len = std::vsnprintf(nullptr, 0, format, arg_copy);
  va_end(arg_copy);

  std::vector<char> buf(len + 1);

  std::vsnprintf(&buf[0], len + 1, format, arg);

  return std::string(&buf[0], &buf[0] + len);
}

std::unique_ptr<Error>
Error::make_cause_unique() const
{
  if (cause_) {
    return std::make_unique<Error>(*cause_.get());
  } else {
    return std::unique_ptr<Error>();
  }
}

}  // namespace zrr
