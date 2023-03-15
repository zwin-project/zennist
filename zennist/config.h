#pragma once

#include "util.h"

namespace zennist {

class Config final
{
 public:
  DISABLE_MOVE_AND_COPY(Config);

  static zrr::Error AutoLoad();

  static const Config &get();

  inline const std::string &wallpaper() const;

 private:
  Config() = default;

  zrr::Result<std::string> GetConfigPath(void) const;

  static Config singleton_;

  std::string wallpaper_;
  bool loaded_ = false;
};

inline const std::string &
Config::wallpaper() const
{
  return wallpaper_;
}

}  // namespace zennist
