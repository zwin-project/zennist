#pragma once

#include "util.h"

namespace zennist {

class XrSystem
{
 public:
  /// The ownership of `proxy` moves
  static std::unique_ptr<XrSystem> New(
      struct zen_xr_system *proxy, uint32_t wl_name, zrr::Error &error);

  DISABLE_MOVE_AND_COPY(XrSystem);

  ~XrSystem();

  void Connect();

  inline uint32_t wl_name() const;

 private:
  /// The ownership of `proxy` moves
  XrSystem(struct zen_xr_system *proxy, uint32_t wl_name);

  zen_xr_system *proxy_;  // nonnull, owning
  const uint32_t wl_name_;
};

inline uint32_t
XrSystem::wl_name() const
{
  return wl_name_;
}

}  // namespace zennist
