#include "config.h"

#include <pwd.h>
#include <tomlc17.h>
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
  toml_result_t config_result = GetTomlResult();
  if (!config_result.ok) {
    ZennistWarn("Config is empty, use the default values.");
    toml_free(config_result);
    return true;
  }

  // Strings in toml_datum_t are owned by toml_result_t
  toml_datum_t favorite_apps = toml_get(config_result.toptab, "favorite_apps");
  if (favorite_apps.type == TOML_ARRAY) {
    table_.favorite_apps.resize(favorite_apps.u.arr.size);
    for (size_t i = 0; i < table_.favorite_apps.size(); i++) {
      table_.favorite_apps[i] = {};
      toml_datum_t favorite_app = favorite_apps.u.arr.elem[i];
      if (favorite_app.type != TOML_TABLE) {
        ZennistError("Each 'favorite_apps' entry must be a table");
        toml_free(config_result);
        return false;
      }

      toml_datum_t name = toml_get(favorite_app, "name");
      if (name.type == TOML_STRING) {
        table_.favorite_apps[i].name = strdup(name.u.s);
      } else {
        table_.favorite_apps[i].name = strdup("");
      }

      toml_datum_t exec = toml_get(favorite_app, "exec");
      if (exec.type == TOML_STRING && strlen(exec.u.s) != 0) {
        table_.favorite_apps[i].exec = strdup(exec.u.s);
      } else {
        ZennistError("Required config key not specified: favorite_apps:exec");
        toml_free(config_result);
        return false;
      }

      toml_datum_t icon = toml_get(favorite_app, "icon");
      if (icon.type == TOML_STRING && strlen(icon.u.s) != 0) {
        table_.favorite_apps[i].icon = strdup(icon.u.s);
      } else {
        table_.favorite_apps[i].icon = strdup("");
        ZennistWarn("Required config key not specified: favorite_apps:icon");
      }

      toml_datum_t disable_2d = toml_get(favorite_app, "disable_2d");
      if (disable_2d.type == TOML_BOOLEAN) {
        table_.favorite_apps[i].disable_2d = disable_2d.u.boolean;
      } else {
        table_.favorite_apps[i].disable_2d = false;
      }

      toml_datum_t disable_3d = toml_get(favorite_app, "disable_3d");
      if (disable_3d.type == TOML_BOOLEAN) {
        table_.favorite_apps[i].disable_3d = disable_3d.u.boolean;
      } else {
        table_.favorite_apps[i].disable_3d = false;
      }
    }
  };

  toml_free(config_result);
  return true;
}

toml_result_t
Config::GetTomlResult()
{
  toml_result_t result = {};

  std::string config_filepath = GetConfigFilePath();
  if (config_filepath.compare("") == 0) {
    ZennistWarn("Could not find the home directory");
    return result;
  }

  FILE *fp = fopen(config_filepath.c_str(), "r");
  if (fp == nullptr) {
    ZennistWarn("Failed to open %s", config_filepath.c_str());
    return result;
  }

  result = toml_parse_file(fp);
  fclose(fp);

  if (!result.ok) ZennistWarn("Failed to parse config.toml: %s", result.errmsg);

  return result;
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
