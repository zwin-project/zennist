#pragma once

#include <zukou.h>

#include "viewer.h"

namespace zennist {

class Roof
{
 public:
  DISABLE_MOVE_AND_COPY(Roof);
  Roof() = delete;
  Roof(zukou::System* system, zukou::VirtualObject* virtual_object);

  bool Render(float radius, glm::mat4 transform, const char* model_path);

 private:
  Viewer viewer_;
};

}  // namespace zennist
