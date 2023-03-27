#include "ui/footer.h"

#include "application.h"
#include "ui/xr-system-button.h"
#include "xr-system.h"

namespace zennist {

Footer::Footer(const Glib::RefPtr<Gdk::Monitor> &gdk_monitor)
{
  auto screen = Gdk::Screen::get_default();

  set_app_paintable(true);

  auto visual = screen->get_rgba_visual();
  gtk_widget_set_visual(GTK_WIDGET(gobj()), visual->gobj());

  gtk_layer_init_for_window(gobj());
  gtk_layer_set_monitor(gobj(), gdk_monitor->gobj());
  gtk_layer_set_layer(gobj(), GTK_LAYER_SHELL_LAYER_TOP);
  gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_RIGHT, true);
  gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_LEFT, true);
  gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_BOTTOM, true);
  gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_TOP, false);

  box_.set_size_request(-1, 45);
  box_.get_style_context()->add_class("footer");

  add(box_);

  show_all();

  Application *app = Application::Get();

  for (auto &xr_system : app->xr_systems()) {
    HandleXrSystemAdded(xr_system->wl_name());
  }

  app->signal_xr_system_added.connect(
      sigc::mem_fun(*this, &Footer::HandleXrSystemAdded));

  app->signal_xr_system_removed.connect(
      sigc::mem_fun(*this, &Footer::HandleXrSystemRemoved));
}

void
Footer::HandleXrSystemAdded(uint32_t wl_name)
{
  auto button = XrSystemButton(wl_name);

  button.set_halign(Gtk::ALIGN_END);

  box_.pack_end(button);

  button.show();

  xr_system_buttons_.push_back(std::move(button));
}

void
Footer::HandleXrSystemRemoved(uint32_t wl_name)
{
  auto result = std::remove_if(xr_system_buttons_.begin(),
      xr_system_buttons_.end(), [wl_name](XrSystemButton &button) {
        return button.wl_name() == wl_name;
      });

  xr_system_buttons_.erase(result, xr_system_buttons_.end());
}

}  // namespace zennist
