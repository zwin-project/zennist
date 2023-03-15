#include "application.h"

#include "config.h"
#include "monitor.h"

namespace zennist {

Application::Application(int &argc, char **&argv)
    : Gtk::Application(argc, argv, "dev.zwin.zennist")
{}

Application::~Application() = default;

void
Application::on_monitor_added(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor)
{
  auto monitor = std::make_unique<Monitor>(gdk_monitor);

  add_window(monitor->window());

  monitors_.emplace_back(std::move(monitor));
}

void
Application::on_monitor_removed(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor)
{
  auto result = std::remove_if(monitors_.begin(), monitors_.end(),
      [&gdk_monitor](std::unique_ptr<Monitor> &monitor) {
        return monitor->gdk_monitor() == gdk_monitor;
      });

  monitors_.erase(result, monitors_.end());
}

void
Application::on_activate()
{
  if (auto err = Config::AutoLoad()) {
    err = zrr::Error(err, "Failed to load config");
    g_critical("%s", err.to_string().c_str());
    return;
  }

  hold();

  auto display = Gdk::Display::get_default();
  auto screen = Gdk::Screen::get_default();

  display->signal_monitor_added().connect(
      sigc::mem_fun(*this, &Application::on_monitor_added));

  display->signal_monitor_removed().connect(
      sigc::mem_fun(*this, &Application::on_monitor_removed));

  int monitor_count = display->get_n_monitors();

  for (int i = 0; i < monitor_count; i++) {
    auto monitor = display->get_monitor(i);
    on_monitor_added(monitor);
  }
}

}  // namespace zennist
