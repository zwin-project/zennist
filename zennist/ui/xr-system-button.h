#pragma once

#include "util.h"

namespace zennist {

class XrSystemButton : public Gtk::Button
{
 public:
  XrSystemButton() = delete;
  explicit XrSystemButton(uint32_t wl_name);

  void on_clicked() override;

  inline uint32_t wl_name();

 private:
  uint32_t wl_name_;
};

inline uint32_t
XrSystemButton::wl_name()
{
  return wl_name_;
}

}  // namespace zennist
