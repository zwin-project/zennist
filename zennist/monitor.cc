#include "monitor.h"

#include "application.h"
#include "config.h"

namespace zennist {

Monitor::Monitor(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor)
    : wallpaper_(gdk_monitor), footer_(gdk_monitor)
{
  Application *app = Application::Get();
  app->add_window(wallpaper_);
  app->add_window(footer_);
}

}  // namespace zennist
