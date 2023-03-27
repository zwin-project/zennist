#include "application.h"

#include "config.h"
#include "monitor.h"
#include "xr-system.h"

namespace zennist {

Application *Application::singleton_ = nullptr;

Application::Application(int &argc, char **&argv)
    : Gtk::Application(argc, argv, "dev.zwin.zennist")
{
  g_assert_null(singleton_);
  singleton_ = this;
}

Application::~Application()
{
  if (registry_) {
    wl_registry_destroy(registry_);
  }
};

Application *
Application::Get()
{
  return singleton_;
}

void
Application::HandleMonitorAdded(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor)
{
  auto monitor = std::make_unique<Monitor>(gdk_monitor);

  monitors_.emplace_back(std::move(monitor));
}

void
Application::HandleMonitorRemoved(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor)
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

  if (auto err = LoadCss()) {
    err = zrr::Error(err, "Failed to load CSS");
    g_critical("%s", err.to_string().c_str());
    return;
  }

  auto display = Gdk::Display::get_default();
  auto screen = Gdk::Screen::get_default();

  auto *wl_display = gdk_wayland_display_get_wl_display(display->gobj());
  registry_ = wl_display_get_registry(wl_display);
  wl_registry_add_listener(registry_, &Application::registry_listener_, this);

  display->signal_monitor_added().connect(
      sigc::mem_fun(*this, &Application::HandleMonitorAdded));

  display->signal_monitor_removed().connect(
      sigc::mem_fun(*this, &Application::HandleMonitorRemoved));

  int monitor_count = display->get_n_monitors();

  for (int i = 0; i < monitor_count; i++) {
    auto monitor = display->get_monitor(i);
    HandleMonitorAdded(monitor);
  }

  hold();
}

zrr::Error
Application::LoadCss()
{
  auto css_provider = Gtk::CssProvider::create();
  auto screen = Gdk::Screen::get_default();

  try {
    css_provider->load_from_path(ZENNIST_CSS_DIR "/main.css");
  } catch (Glib::Error e) {
    return zrr::Error("%s", e.what().c_str());
  }

  auto context = Gtk::StyleContext::create();

  context->add_provider_for_screen(
      screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  return zrr::Error::empty();
}

const wl_registry_listener Application::registry_listener_ = {
    Application::HandleGlobal,
    Application::HandleGlobalRemove,
};  // namespace zennist

void
Application::HandleGlobal(void *data, struct wl_registry *wl_registry,
    uint32_t name, const char *interface, uint32_t version)
{
  auto *self = static_cast<Application *>(data);
  if (std::strcmp(interface, "zen_xr_system") == 0) {
    auto *proxy = static_cast<struct zen_xr_system *>(
        wl_registry_bind(wl_registry, name, &zen_xr_system_interface, version));

    auto error = zrr::Error::empty();
    auto xr_system = XrSystem::New(proxy, name, error);
    if (!xr_system) {
      error = zrr::Error(error, "Failed to create XrSystem");
      g_critical("%s", error.to_string().c_str());
      return;
    }

    uint32_t wl_name = xr_system->wl_name();

    self->xr_systems_.push_back(std::move(xr_system));

    self->signal_xr_system_added.emit(wl_name);
  }
}

void
Application::HandleGlobalRemove(
    void *data, struct wl_registry * /*wl_registry*/, uint32_t name)
{
  auto *self = static_cast<Application *>(data);
  auto result = std::remove_if(self->xr_systems_.begin(),
      self->xr_systems_.end(), [name](std::unique_ptr<XrSystem> &xr_system) {
        return xr_system->wl_name() == name;
      });

  std::vector<std::unique_ptr<XrSystem>> systems_to_remove(
      std::make_move_iterator(result),
      std::make_move_iterator(self->xr_systems_.end()));

  self->xr_systems_.erase(result, self->xr_systems_.end());

  for (auto &system : systems_to_remove) {
    self->signal_xr_system_removed.emit(system->wl_name());
  }
}

}  // namespace zennist
