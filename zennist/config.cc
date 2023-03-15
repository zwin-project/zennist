#include "config.h"

namespace zennist {

zrr::Error
Config::AutoLoad()
{
  auto &self = Config::singleton_;

  if (self.loaded_) {
    return zrr::Error::empty();
  }

  auto config_path = self.GetConfigPath();
  if (auto &err = config_path.error()) {
    return zrr::Error(err, "Failed to find a config file path");
  }

  auto result = toml::parse_file(*config_path);
  if (!result) {
    std::stringstream ss;

    ss << result.error();

    auto cause = zrr::Error("%s", ss.str().c_str());
    return zrr::Error(cause, "Failed to parse %s", config_path->c_str());
  }

  g_debug("Config file : '%s'", config_path->c_str());

  auto wallpaper = result["zennist"]["wallpaper"];

  if (wallpaper.is_string()) {
    self.wallpaper_ = wallpaper.as_string()->get();
  } else {
    if (!wallpaper) {
      return zrr::Error("wallpaper is not found");
    } else {
      return zrr::Error("wallpaper is not string");
    }
  }

  return zrr::Error::empty();
}

const Config &
Config::get()
{
  return singleton_;
}

zrr::Result<std::string>
Config::GetConfigPath() const
{
  const std::string config_dir_partial_path = "/.config";
  const std::string toml_file_partial_path = "/zen-desktop/config.toml";

  const char *config_home_env = std::getenv("XDG_CONFIG_HOME");

  std::string config_path;

  if (config_home_env == nullptr) {
    const char *homedir_env = std::getenv("HOME");
    if (homedir_env == nullptr) {
      homedir_env = getpwuid(getuid())->pw_dir;
    }

    if (homedir_env == nullptr) {
      return zrr::Error("Could not find a home directory");
    }

    config_path +=
        homedir_env + config_dir_partial_path + toml_file_partial_path;
  } else {
    config_path += config_home_env + toml_file_partial_path;
  }

  return config_path;
}

Config Config::singleton_;

}  // namespace zennist
