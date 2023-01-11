#pragma once

#include <zukou.h>

#include "texture.h"

namespace zennist {

class JpegTexture final : public Texture
{
 public:
  DISABLE_MOVE_AND_COPY(JpegTexture);
  JpegTexture(zukou::System *system);
  ~JpegTexture();

  bool Load(const char *filename);

 private:
  bool loaded_ = false;

  int fd_ = 0;
  zukou::ShmPool pool_;
  zukou::Buffer texture_buffer_;
};

}  // namespace zennist
