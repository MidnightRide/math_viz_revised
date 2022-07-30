/* date = June 30th 2022 4:05 pm */

#ifndef RENDERER_H
#define RENDERER_H

#include "../util/util.h"
#include "shader.h"
#include "gfx.h"
#include "vao.h"
#include "vbo.h"
#include "texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H

enum FillMode {
    FillMode_fill, FillMode_line,
};

enum ShaderType {
    ShaderType_none,
    ShaderType_basic_texture,
    ShaderType_basic_color,
    ShaderType_text,
    ShaderType_MAX,
};

enum Textures {
    Textures_MAX,
};

#define CAMERA_STACK_MAX 256

enum RenderPass {
    RenderPass_2d,
    RenderPass_3d,
};

struct Character {
    struct Texture texture;
    ivec2s size;
    ivec2s bearing;
    u32 advance;
};

struct Renderer {
    enum CameraType camera_type;
    
    struct {
        enum CameraType array[CAMERA_STACK_MAX];
        u64 size;
    } camera_stack;
    
    struct PerspectiveCamera perspective_camera;
    struct OrthoCamera ortho_camera;
    
    struct Shader shaders[ShaderType_MAX];
    enum ShaderType current_shader;
    struct Shader shader;
    
    struct Texture textures[Textures_MAX];
    
    vec4s clear_color;
    
    struct VBO vbo, ibo;
    struct VAO vao;
    
    struct Character characters[128];
    
    FT_Library ft;
    
    struct {
        struct VAO vao;
        struct VBO vbo;
    } text;
    
    struct {
        b8 wireframe : 1;
    } flags;
};

void renderer_init(struct Renderer *self);
void renderer_destroy(struct Renderer *self);
void renderer_update(struct Renderer *self);
void renderer_prepare(struct Renderer *self, enum RenderPass pass);
void renderer_set_camera(struct Renderer *self, enum CameraType type);
void renderer_push_camera(struct Renderer *self);
void renderer_pop_camera(struct Renderer *self);
void renderer_set_view_proj(struct Renderer *self);
void renderer_use_shader(struct Renderer *self, enum ShaderType shader);
void renderer_quad_color(struct Renderer *self, vec2s size, vec4s color, mat4s model);
void renderer_quad_texture(struct Renderer *self, struct Texture texture, vec2s size, vec4s color, vec2s uv_min, vec2s uv_max, mat4s model);
void renderer_text(struct Renderer *self, char *text, u32 text_length, vec2s position, f32 scale, vec3s color);
void renderer_aabb(struct Renderer *self, AABB aabb, vec4s color, mat4s model, enum FillMode fill_mode);

#endif //RENDERER_H
