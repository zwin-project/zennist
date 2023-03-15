#include "monitor.h"

#include "config.h"

namespace zennist {

Monitor::Monitor(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor)
    : gdk_monitor_(gdk_monitor), wallpaper_()
{
  gdk_monitor->connect_property_changed(
      "geometry", sigc::mem_fun(*this, &Monitor::on_geometry_changed));

  gtk_layer_init_for_window(window_.gobj());
  gtk_layer_set_monitor(window_.gobj(), gdk_monitor->gobj());
  gtk_layer_set_layer(window_.gobj(), GTK_LAYER_SHELL_LAYER_BACKGROUND);

  UpdateWallpaper();
}

void
Monitor::UpdateWallpaper()
{
  Gdk::Rectangle geometry;
  gdk_monitor_->get_geometry(geometry);

  window_.remove();

  if (geometry.has_zero_area()) {
    return;
  }

  auto &config = Config::get();

  try {
    Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create_from_file(
        config.wallpaper(), geometry.get_width(), geometry.get_height());

    wallpaper_.set(buf);

    window_.add(wallpaper_);
  } catch (Glib::Error err) {
    std::string error_log;
    error_log += "(Zennist) Failed to load a wallpaper.\n";
    error_log += "Cause: " + err.what();
    error_label_.set_label(error_log);
    error_label_.set_justify(Gtk::JUSTIFY_CENTER);

    window_.add(error_label_);
  }

  window_.show_all();
}

void
Monitor::on_geometry_changed()
{
  UpdateWallpaper();
}

}  // namespace zennist
