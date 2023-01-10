#pragma once

#include <png.h>
#include <stdio.h>
#include <string.h>
#include <zukou.h>

#include "texture.h"

namespace zennist {

class PngTexture final : public zukou::GlTexture, public Texture
{
 public:
  DISABLE_MOVE_AND_COPY(PngTexture);
  PngTexture(zukou::System *system);
  ~PngTexture();

  bool Init();

  bool Load(const char *filename);

 private:
  bool loaded_ = false;

  int fd_ = 0;
  zukou::ShmPool pool_;
  zukou::Buffer texture_buffer_;
};

}  // namespace zennist
