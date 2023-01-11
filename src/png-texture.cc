#include "png-texture.h"

#include <png.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SIGNATURE_NUM 8

namespace zennist {

PngTexture::PngTexture(zukou::System *system) : Texture(system), pool_(system)
{}

PngTexture::~PngTexture()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

bool
PngTexture::Load(const char *texture_path)
{
  FILE *fp = nullptr;
  uint32_t width, height;
  int channel;
  size_t size;
  uint32_t read_size;

  unsigned char **rows = nullptr;
  unsigned char *data = nullptr;
  png_struct *png;
  png_info *info;
  png_byte type, depth, compression, interlace, filter;
  png_byte signature[8];

  bool result = false;

  if (loaded_) {
    fprintf(stderr, "PngTexture::Load -- texture already loaded\n");
    goto err;
  }

  fp = fopen(texture_path, "rb");
  if (!fp) {
    fprintf(stderr, "Fail to open file: %s\n", texture_path);
    goto error_open_file;
  }

  read_size = fread(signature, 1, SIGNATURE_NUM, fp);

  if (png_sig_cmp(signature, 0, SIGNATURE_NUM)) {
    fprintf(stderr, "File is not png format.\n");
    goto error_not_png;
  }

  png =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png == nullptr) {
    fprintf(stderr, "Fail to crate png struct.\n");
    goto error_create_png_struct;
  }

  info = png_create_info_struct(png);
  if (info == nullptr) {
    fprintf(stderr, "Fail to crate png image info.\n");
    goto error_create_info;
  }

  png_init_io(png, fp);
  png_set_sig_bytes(png, read_size);
  png_read_png(
      png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, nullptr);

  width = png_get_image_width(png, info);
  height = png_get_image_height(png, info);
  depth = png_get_bit_depth(png, info);
  type = png_get_color_type(png, info);
  compression = png_get_compression_type(png, info);
  interlace = png_get_interlace_type(png, info);
  filter = png_get_filter_type(png, info);

  // only support 8bit color depth image
  if (depth != 8) {
    fprintf(
        stderr, "Unsupported PNG format. We support only 8 bit color depth.\n");
    goto error_invalid_png_format;
  }

  if (compression != PNG_COMPRESSION_TYPE_BASE) {
    fprintf(stderr, "Unsupported PNG format.\n");
    goto error_invalid_png_format;
  }
  if (interlace != PNG_INTERLACE_NONE) {
    fprintf(stderr, "Unsupported PNG format.\n");
    goto error_invalid_png_format;
  }
  if (filter != PNG_FILTER_TYPE_BASE) {
    fprintf(stderr, "Unsupported PNG format.\n");
    goto error_invalid_png_format;
  }

  // only RGB and RGBA support for now
  if (type == PNG_COLOR_TYPE_RGB)
    channel = 3;
  else if (type == PNG_COLOR_TYPE_RGB_ALPHA)
    channel = 4;
  else {
    fprintf(stderr,
        "Unsupported PNG format. We support only RGB and RGBA format. (%u)\n",
        type);
    goto error_invalid_png_format;
  }

  size = channel * width * height;

  fd_ = zukou::Util::CreateAnonymousFile(size);
  if (fd_ <= 0) goto err;
  if (!pool_.Init(fd_, size)) goto err;
  if (!texture_buffer_.Init(&pool_, 0, size)) goto err;

  {
    rows = png_get_rows(png, info);
    data = static_cast<uint8_t *>(
        mmap(nullptr, size, PROT_WRITE, MAP_SHARED, fd_, 0));

    for (uint32_t i = 0; i < height; i++) {
      memcpy(data + i * width * channel, rows[i], width * channel);
    }

    munmap(data, size);
  }

  if (type == PNG_COLOR_TYPE_RGB)
    Image2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
        GL_UNSIGNED_BYTE, &texture_buffer_);
  if (type == PNG_COLOR_TYPE_RGB_ALPHA)
    Image2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, &texture_buffer_);

  loaded_ = true;
  result = true;

err:
error_invalid_png_format:
error_create_info:
  png_destroy_read_struct(&png, &info, nullptr);

error_create_png_struct:
error_not_png:
  fclose(fp);

error_open_file:
  return result;
}

}  // namespace zennist
