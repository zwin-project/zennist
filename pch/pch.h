#pragma once

#include <gdk/gdkwayland.h>
#include <gdkmm/display.h>
#include <gdkmm/monitor.h>
#include <gtk-layer-shell.h>
#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>
#include <pwd.h>
#include <toml++/toml.h>
#include <wayland-client-core.h>
#include <zen-desktop-client-protocol.h>
#include <zrr/zrr.h>

#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
