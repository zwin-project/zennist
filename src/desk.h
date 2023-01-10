#pragma once

#include <zukou.h>

#include "viewer.h"

namespace zennist {

class Desk
{
 public:
  DISABLE_MOVE_AND_COPY(Desk);
  Desk() = delete;
  Desk(zukou::System* system, zukou::VirtualObject* virtual_object);

  bool Render(float radius, glm::mat4 transform, const char* model_path);

 private:
  Viewer viewer_;
};

}  // namespace zennist
