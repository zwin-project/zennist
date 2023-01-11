#pragma once

#include <string.h>
#include <toml.h>
#include <zukou.h>

#include <vector>

namespace zennist {

struct FavoriteApp {
  char *name;  // optional
  char *exec;  // required
  char *icon;  // required
  bool disable_2d;
  bool disable_3d;
};

struct ConfigTable {};

class Config
{
 public:
  DISABLE_MOVE_AND_COPY(Config);
  Config();

  std::vector<FavoriteApp> GetFavoriteApps();
  bool Load();

 private:
  struct toml_table_t *GetTomlTable();
  std::string GetConfigFilePath();

  struct {
    std::vector<FavoriteApp> favorite_apps;
  } table_;
};

}  // namespace zennist
