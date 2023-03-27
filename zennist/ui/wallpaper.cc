#include "ui/wallpaper.h"

#include "config.h"

namespace zennist {

Wallpaper::Wallpaper(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor)
    : gdk_monitor_(gdk_monitor)
{
  gdk_monitor->connect_property_changed(
      "geometry", sigc::mem_fun(*this, &Wallpaper::HandleGeometryChanged));

  gtk_layer_init_for_window(gobj());
  gtk_layer_set_monitor(gobj(), gdk_monitor->gobj());
  gtk_layer_set_layer(gobj(), GTK_LAYER_SHELL_LAYER_BACKGROUND);

  UpdateWallpaper();
};

void
Wallpaper::UpdateWallpaper()
{
  Gdk::Rectangle geometry;
  gdk_monitor_->get_geometry(geometry);

  remove();

  if (geometry.has_zero_area()) {
    return;
  }

  auto &config = Config::Get();

  try {
    Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create_from_file(
        config.wallpaper(), geometry.get_width(), geometry.get_height());

    image_.set(buf);

    add(image_);
  } catch (Glib::Error err) {
    std::string error_log;
    error_log += "(Zennist) Failed to load a wallpaper.\n";
    error_log += "Cause: " + err.what();
    error_label_.set_label(error_log);
    error_label_.set_justify(Gtk::JUSTIFY_CENTER);

    add(error_label_);
  }

  show_all();
}

void
Wallpaper::HandleGeometryChanged()
{
  UpdateWallpaper();
}

}  // namespace zennist
