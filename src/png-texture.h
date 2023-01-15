#pragma once

#include <zukou.h>

#include "texture.h"

namespace zennist {

class PngTexture final : public Texture
{
 public:
  DISABLE_MOVE_AND_COPY(PngTexture);
  PngTexture(zukou::System *system, const char *texture_path);
  ~PngTexture();

  bool Load();

 private:
  bool loaded_ = false;

  int fd_ = 0;
  zukou::ShmPool pool_;
  zukou::Buffer texture_buffer_;

  const char *texture_path_;
};

}  // namespace zennist
