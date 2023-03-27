#pragma once

#include "util.h"

namespace zennist {

class Wallpaper : public Gtk::Window
{
 public:
  Wallpaper() = delete;
  explicit Wallpaper(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor);

 private:
  void UpdateWallpaper();

  void HandleGeometryChanged();

  const Glib::RefPtr<Gdk::Monitor> gdk_monitor_;
  Gtk::Image image_;
  Gtk::Label error_label_;
};

}  // namespace zennist
