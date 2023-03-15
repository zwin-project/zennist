#pragma once

#include "util.h"

namespace zennist {

class Monitor
{
 public:
  DISABLE_MOVE_AND_COPY(Monitor);

  Monitor() = delete;
  explicit Monitor(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor);

  ~Monitor() = default;

  inline const Glib::RefPtr<Gdk::Monitor> &gdk_monitor() const;

  inline Gtk::Window &window();

 private:
  void UpdateWallpaper();

  void on_geometry_changed();

  const Glib::RefPtr<Gdk::Monitor> gdk_monitor_;
  Gtk::Window window_;
  Gtk::Image wallpaper_;
  Gtk::Label error_label_;
};

inline const Glib::RefPtr<Gdk::Monitor> &
Monitor::gdk_monitor() const
{
  return gdk_monitor_;
}

inline Gtk::Window &
Monitor::window()
{
  return window_;
}

}  // namespace zennist
