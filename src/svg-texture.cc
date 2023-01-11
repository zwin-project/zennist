#include "svg-texture.h"

#include <cairo.h>
#include <librsvg/rsvg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <iostream>

#define SIGNATURE_NUM 8

namespace zennist {

SvgTexture::SvgTexture(zukou::System *system) : Texture(system), pool_(system)
{}

SvgTexture::~SvgTexture()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

bool
SvgTexture::Load(const char *texture_path)
{
  int channel = 4;
  uint32_t width = 400;
  uint32_t height = 400;
  size_t size = channel * width * height;

  cairo_surface_t *surface =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

  if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
    std::cerr << "Failed to create cairo_surface" << std::endl;
    return false;
  }

  cairo_t *cr = cairo_create(surface);
  if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
    std::cerr << "Failed to create cairo_t" << std::endl;
    return false;
  }

  cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
  cairo_paint(cr);

  GError *error = NULL;
  GFile *file = g_file_new_for_path(texture_path);
  RsvgHandle *handle = rsvg_handle_new_from_gfile_sync(
      file, RSVG_HANDLE_FLAGS_NONE, NULL, &error);
  if (handle == NULL) {
    std::cerr << "Failed to create the svg handler: " << error->message
              << std::endl;
    g_error_free(error);
    return false;
  }

  double x = 0;
  double y = 0;
  RsvgRectangle viewport = {
      x,
      y,
      static_cast<double>(width),
      static_cast<double>(height),
  };

  if (!rsvg_handle_render_document(handle, cr, &viewport, &error)) {
    std::cerr << "Failed to render the svg: " << error->message << std::endl;
    g_error_free(error);
    return false;
  }
  g_object_unref(handle);

  fd_ = zukou::Util::CreateAnonymousFile(size);
  if (fd_ <= 0) return false;
  if (!pool_.Init(fd_, size)) return false;
  if (!texture_buffer_.Init(&pool_, 0, size)) return false;

  {
    unsigned char *data = cairo_image_surface_get_data(surface);
    std::vector<unsigned char> src(size);

    for (size_t i = 0; i < height; ++i) {
      for (size_t j = 0; j < width; ++j) {
        int idx = channel * width * i + channel * j;

        src[idx + 0] = *((data + idx) + 2);
        src[idx + 1] = *((data + idx) + 1);
        src[idx + 2] = *((data + idx) + 0);
        src[idx + 3] = *((data + idx) + 3);
      }
    }
    unsigned char *dst = static_cast<uint8_t *>(
        mmap(nullptr, size, PROT_WRITE, MAP_SHARED, fd_, 0));

    memcpy(dst, src.data(), size);
    munmap(dst, size);
  }

  Image2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, &texture_buffer_);

  loaded_ = true;

  // TODO: destory resources

  return true;
}

}  // namespace zennist
