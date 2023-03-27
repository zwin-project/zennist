#pragma once

#include "util.h"
#include "xr-system-button.h"

namespace zennist {

class XrSystemButton;

class Footer : public Gtk::Window
{
 public:
  Footer() = delete;
  explicit Footer(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor);

 private:
  void HandleXrSystemAdded(uint32_t wl_name);
  void HandleXrSystemRemoved(uint32_t wl_name);

  Gtk::HBox box_;
  std::vector<XrSystemButton> xr_system_buttons_;
};

}  // namespace zennist
