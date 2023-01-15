#pragma once

#include <zukou.h>

#include "texture.h"

namespace zennist {

class SvgTexture final : public Texture
{
 public:
  DISABLE_MOVE_AND_COPY(SvgTexture);
  SvgTexture(zukou::System *system, const char *texture_path);
  ~SvgTexture();

  bool Load();

 private:
  bool loaded_ = false;

  int fd_ = 0;
  zukou::ShmPool pool_;
  zukou::Buffer texture_buffer_;

  const char *texture_path_;
};

}  // namespace zennist
