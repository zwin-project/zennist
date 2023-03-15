#pragma once

#include "util.h"

namespace zennist {

class Monitor;

class Application final : public Gtk::Application
{
 public:
  DISABLE_MOVE_AND_COPY(Application);

  explicit Application(int &argc, char **&argv);
  virtual ~Application();

 private:
  void on_activate() override;

  void on_monitor_added(const Glib::RefPtr<Gdk::Monitor> &monitor);

  void on_monitor_removed(const Glib::RefPtr<Gdk::Monitor> &monitor);

  std::vector<std::unique_ptr<Monitor>> monitors_;
};

}  // namespace zennist
