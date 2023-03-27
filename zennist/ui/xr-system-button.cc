#include "ui/xr-system-button.h"

#include "application.h"
#include "xr-system.h"

namespace zennist {

XrSystemButton::XrSystemButton(uint32_t wl_name) : wl_name_(wl_name) {}

void
XrSystemButton::on_clicked()
{
  Application *app = Application::Get();

  for (auto &xr_system : app->xr_systems()) {
    if (xr_system->wl_name() == wl_name_) {
      xr_system->Connect();
      return;
    }
  }
}

}  // namespace zennist
