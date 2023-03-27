#include "xr-system.h"

namespace zennist {

std::unique_ptr<XrSystem>
XrSystem::New(zen_xr_system *proxy, uint32_t wl_name, zrr::Error & /*error*/)
{
  return std::unique_ptr<XrSystem>(new XrSystem(proxy, wl_name));
}

XrSystem::XrSystem(zen_xr_system *proxy, uint32_t wl_name)
    : proxy_(proxy), wl_name_(wl_name)
{}

XrSystem::~XrSystem() { zen_xr_system_destroy(proxy_); }

void
XrSystem::Connect()
{
  zen_xr_system_connect(proxy_);
}

}  // namespace zennist
