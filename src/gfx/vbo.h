/* date = June 30th 2022 4:17 pm */

#ifndef VBO_H
#define VBO_H

#include "../util/util.h"
#include "gfx.h"

struct VBO
{
    GLuint handle;
    GLint type;
    b8 dynamic;
};

struct VBO vbo_create(GLint type, b8 dynamic);
void vbo_destroy(struct VBO self);
void vbo_bind(struct VBO self);
void vbo_buffer(struct VBO self, void *data, u64 offset, u64 count);

#endif //VBO_H
