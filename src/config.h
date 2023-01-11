#pragma once

#include <zukou.h>

namespace zennist {

class Config
{
 public:
  DISABLE_MOVE_AND_COPY(Config);
  Config();

  bool Load(const char* path);

 private:
};

}  // namespace zennist
