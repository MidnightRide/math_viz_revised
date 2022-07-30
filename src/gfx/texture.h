/* date = July 3rd 2022 3:30 pm */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "gfx.h"
#include "../util/util.h"

struct Texture {
    GLuint handle;
    ivec2s size;
    vec2s uv_unit;
};

void texture_load_pixels(char *path, u8 **pixels_out, u64 *width_out, u64 *height_out);
struct Texture texture_create_from_pixels(u8 *pixels, u64 width, u64 height, GLint internal_format, GLint format);
struct Texture texture_create_from_path(char *path);
void texture_destroy(struct Texture self);
void texture_bind(struct Texture self);

#endif //TEXTURE_H
