#pragma once

#include "util.h"

namespace zennist {

class Monitor;
class XrSystem;

class Application final : public Gtk::Application
{
 public:
  DISABLE_MOVE_AND_COPY(Application);

  explicit Application(int &argc, char **&argv);
  virtual ~Application();

  static Application *Get();

  inline std::vector<std::unique_ptr<XrSystem>> &xr_systems();

  sigc::signal<void, uint32_t /*wl_name*/> signal_xr_system_added;
  sigc::signal<void, uint32_t /*wl_name*/> signal_xr_system_removed;

 private:
  static Application *singleton_;
  static const wl_registry_listener registry_listener_;

  static void HandleGlobal(void *data, struct wl_registry *wl_registry,
      uint32_t name, const char *interface, uint32_t version);

  static void HandleGlobalRemove(
      void *data, struct wl_registry *wl_registry, uint32_t name);

  void on_activate() override;

  void HandleMonitorAdded(const Glib::RefPtr<Gdk::Monitor> &monitor);

  void HandleMonitorRemoved(const Glib::RefPtr<Gdk::Monitor> &monitor);

  zrr::Error LoadCss();

  std::vector<std::unique_ptr<Monitor>> monitors_;
  std::vector<std::unique_ptr<XrSystem>> xr_systems_;
  wl_registry *registry_ = nullptr;  // nonnull after on_active
};

inline std::vector<std::unique_ptr<XrSystem>> &
Application::xr_systems()
{
  return xr_systems_;
}

}  // namespace zennist
