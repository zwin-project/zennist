#include "config.h"

#include <pwd.h>
#include <toml.h>
#include <unistd.h>

#include "log.h"

namespace zennist {

Config::Config() {}

std::vector<FavoriteApp>
Config::GetFavoriteApps()
{
  return table_.favorite_apps;
}

bool
Config::Load()
{
  struct toml_table_t *config_table = GetTomlTable();
  if (config_table == nullptr) {
    ZennistWarn("Config is empty, use the default values.");
    return true;
  }

  toml_array_t *favorite_apps = toml_array_in(config_table, "favorite_apps");
  if (favorite_apps != nullptr) {
    table_.favorite_apps.resize(toml_array_nelem(favorite_apps));
    for (size_t i = 0; i < table_.favorite_apps.size(); i++) {
      table_.favorite_apps[i] = {};
      auto favorite_app = toml_table_at(favorite_apps, i);

      toml_datum_t name = toml_string_in(favorite_app, "name");
      if (name.ok) {
        table_.favorite_apps[i].name = name.u.s;
      } else {
        char empty_name[] = "";
        table_.favorite_apps[i].name = empty_name;
      }

      toml_datum_t exec = toml_string_in(favorite_app, "exec");
      if (exec.ok && strlen(exec.u.s) != 0) {
        table_.favorite_apps[i].exec = exec.u.s;
      } else {
        ZennistError("Required config key not specified: favorite_apps:exec");
        return false;
      }

      toml_datum_t icon = toml_string_in(favorite_app, "icon");
      if (icon.ok && strlen(icon.u.s) != 0) {
        table_.favorite_apps[i].icon = icon.u.s;
      } else {
        char empty_icon[] = "";
        table_.favorite_apps[i].icon = empty_icon;
        ZennistWarn("Required config key not specified: favorite_apps:icon");
      }

      toml_datum_t disable_2d = toml_bool_in(favorite_app, "disable_2d");
      if (disable_2d.ok) {
        table_.favorite_apps[i].disable_2d = disable_2d.u.b;
      } else {
        table_.favorite_apps[i].disable_2d = false;
      }

      toml_datum_t disable_3d = toml_bool_in(favorite_app, "disable_3d");
      if (disable_3d.ok) {
        table_.favorite_apps[i].disable_3d = disable_3d.u.b;
      } else {
        table_.favorite_apps[i].disable_3d = false;
      }
    }
  };

  return true;
}

struct toml_table_t *
Config::GetTomlTable()
{
  std::string config_filepath = GetConfigFilePath();
  if (config_filepath.compare("") == 0) {
    ZennistWarn("Could not find the home directory");
    return nullptr;
  }

  FILE *fp = fopen(config_filepath.c_str(), "r");
  if (fp == nullptr) {
    ZennistWarn("Failed to open %s", config_filepath.c_str());
    return nullptr;
  }

  char errbuf[200];
  toml_table_t *tbl = toml_parse_file(fp, errbuf, sizeof(errbuf));
  fclose(fp);

  if (tbl == nullptr) ZennistWarn("Failed to parse config.toml: %s", errbuf);

  return tbl;
}

std::string
Config::GetConfigFilePath()
{
  std::string config_dir_partial_path("/.config");
  std::string toml_file_partial_path("/zen-desktop/config.toml");
  std::string config_home("");
  char *config_home_env = getenv("XDG_CONFIG_HOME");

  if (config_home_env == nullptr) {
    char *homedir_env = getenv("HOME");
    if (homedir_env == nullptr) homedir_env = getpwuid(getuid())->pw_dir;
    if (homedir_env == nullptr) {
      return "";
    }

    config_home =
        homedir_env + config_dir_partial_path + toml_file_partial_path;
  } else {
    config_home = config_home_env + toml_file_partial_path;
  }
  return config_home;
}

}  // namespace zennist
