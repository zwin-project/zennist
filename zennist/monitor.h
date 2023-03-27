#pragma once

#include "ui/footer.h"
#include "ui/wallpaper.h"
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

 private:
  const Glib::RefPtr<Gdk::Monitor> gdk_monitor_;
  Wallpaper wallpaper_;
  Footer footer_;
};

inline const Glib::RefPtr<Gdk::Monitor> &
Monitor::gdk_monitor() const
{
  return gdk_monitor_;
}

}  // namespace zennist
