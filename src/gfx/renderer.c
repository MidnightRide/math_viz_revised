#include "renderer.h"
#include "../state.h"

static inline void renderer_load_faces(struct Renderer *self, char *font_path) {
    FT_Face face;
    if (FT_New_Face(self->ft, font_path, 0, &face)) {
        printf("ERROR::FREETYPE: Failed to load font");
        return;
    } else {
        FT_Set_Pixel_Sizes(face, 0, 48);
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        for (u8 c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                printf("ERROR::FREETYTPE: Failed to load Glyph");
                continue;
            }
            
            struct Texture texture = texture_create_from_pixels(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_RED);
            
            
            struct Character character = {
                texture, 
                (ivec2s) {{ face->glyph->bitmap.width, face->glyph->bitmap.rows }},
                (ivec2s) {{ face->glyph->bitmap_left, face->glyph->bitmap_top }},
                face->glyph->advance.x
            };
            
            self->characters[c] = character;
        }
    }
    FT_Done_Face(face);
}

static inline void renderer_init_text(struct Renderer *self, char *font_path) {
    if (FT_Init_FreeType(&self->ft)) {
        printf("ERROR::FREETYPE: Could not init FreeType Library");
        return;
    }
    renderer_load_faces(self, font_path);
    FT_Done_FreeType(self->ft);
    
    self->text.vao = vao_create();
    self->text.vbo = vbo_create(GL_ARRAY_BUFFER, true);
    vbo_buffer(self->text.vbo, NULL, 0, sizeof(f32) * 6 * 4);
    vao_attr(self->text.vao, self->text.vbo, 0, 4, GL_FLOAT, 4 * sizeof(f32), 0);
}

void renderer_init(struct Renderer *self) {
    *self = (struct Renderer) {};
    self->camera_type = CameraType_perspective;
    self->current_shader = ShaderType_none;
    
    self->shaders[ShaderType_basic_texture] = shader_create("res/shaders/basic_texture.vs", 
                                                            "res/shaders/basic_texture.fs",
                                                            2, (struct VertexAttr[]) {
                                                                { .index = 0, .name = "position" },
                                                                { .index = 1, .name = "uv" }
                                                            });
    
    self->shaders[ShaderType_basic_color] = shader_create("res/shaders/basic_color.vs", 
                                                          "res/shaders/basic_color.fs",
                                                          1, (struct VertexAttr[]) {
                                                              { .index = 0, .name = "position" },
                                                          });
    self->shaders[ShaderType_text] = shader_create("res/shaders/text.vs",
                                                   "res/shaders/text.fs",
                                                   1, (struct VertexAttr[]) {
                                                       { .index = 0, .name = "vertex" },
                                                   });
    
    self->vao = vao_create();
    self->vbo = vbo_create(GL_ARRAY_BUFFER, true);
    self->ibo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, true);
    
    perspective_camera_init(&self->perspective_camera, radians(75.0f));
    ortho_camera_init(&self->ortho_camera, (vec2s) {{ -state.window->size.x/2.f, -state.window->size.y/2.f}}, (vec2s) {{ state.window->size.x/2.f, state.window->size.y/2.f}});
    
    renderer_init_text(self, "res/fonts/Roboto-Mono.ttf");
}

void renderer_destroy(struct Renderer *self) {
    for (u64 i = 0; i < ShaderType_MAX; ++i) {
        shader_destroy(self->shaders[i]);
    }
    
    vao_destroy(self->vao);
    vbo_destroy(self->vbo);
    vbo_destroy(self->ibo);
}

void renderer_update(struct Renderer *self) {
}

void renderer_prepare(struct Renderer *self, enum RenderPass pass) {
    switch(pass)
    {
        case RenderPass_2d: {
            ortho_camera_init(&self->ortho_camera, (vec2s) {{ -state.window->size.x/2.f, -state.window->size.y/2.f}}, (vec2s) {{ state.window->size.x/2.f, state.window->size.y/2.f}});
            glClear(GL_DEPTH_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_MULTISAMPLE); 
        } break;
        
        case RenderPass_3d: {
            glClearColor(self->clear_color.x, self->clear_color.y,
                         self->clear_color.z, self->clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, self->flags.wireframe ? GL_LINE : GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_MULTISAMPLE);  
        } break;
    }
}

void renderer_set_camera(struct Renderer *self, enum CameraType type) {
    self->camera_type = type;
}

void renderer_push_camera(struct Renderer *self) {
    assert(self->camera_stack.size + 1 < CAMERA_STACK_MAX);
    self->camera_stack.array[self->camera_stack.size++] = self->camera_type;
}

void renderer_pop_camera(struct Renderer *self) {
    assert(self->camera_stack.size != 0);
    self->camera_type = self->camera_stack.array[--self->camera_stack.size];
}

void renderer_set_view_proj(struct Renderer *self) {
    struct ViewProj view_proj;
    
    switch (self->camera_type) {
        case CameraType_perspective: {
            view_proj = self->perspective_camera.view_proj;
        } break;
        
        case CameraType_ortho: {
            view_proj = self->ortho_camera.view_proj;
        } break;
    }
    
    shader_uniform_view_proj(self->shader, view_proj);
}

void renderer_use_shader(struct Renderer *self, enum ShaderType shader) {
    if (shader == self->current_shader) return;
    
    self->current_shader = shader;
    self->shader = self->shaders[shader];
    shader_bind(self->shader);
}

void renderer_quad_color(struct Renderer *self, vec2s size, vec4s color, mat4s model) {
    renderer_use_shader(self, ShaderType_basic_color);
    renderer_set_view_proj(self);
    shader_uniform_mat4(self->shader, "m", model);
    shader_uniform_vec4(self->shader, "color", color);
    
    vbo_buffer(self->vbo, (f32[]) {
                   0, 0, 0,
                   0, size.y, 0,
                   size.x, size.y, 0,
                   size.x, 0, 0,
               }, 0, (4 * 3) * sizeof(f32));
    
    vbo_buffer(self->ibo, (u32[]) {
                   3, 0, 1, 3, 1, 2,
               }, 0, 6 * sizeof(u32));
    
    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, 0, 0);
    
    vao_bind(self->vao);
    vbo_bind(self->ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
}

void renderer_quad_texture(struct Renderer *self, struct Texture texture, vec2s size, vec4s color, vec2s uv_min, vec2s uv_max, mat4s model) {
    renderer_use_shader(self, ShaderType_basic_texture);
    renderer_set_view_proj(self);
    shader_uniform_mat4(self->shader, "m", model);
    shader_uniform_texture2D(self->shader, "tex", texture, 0);
    shader_uniform_vec4(self->shader, "color", color);
    
    vbo_buffer(self->vbo, (f32[]) {
                   0, 0, 0,
                   0, size.y, 0,
                   size.x, size.y, 0,
                   size.x, 0, 0,
                   
                   uv_min.x, uv_min.y,
                   uv_min.x, uv_max.y,
                   uv_max.x, uv_max.y,
                   uv_max.x, uv_min.y
               }, 0, ((4 * 3) + (4 * 2)) * sizeof(f32));
    
    vbo_buffer(self->ibo, (u32[]) {
                   3, 0, 1, 3, 1, 2
               }, 0, 6 * sizeof(u32));
    
    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, 0, 0);
    vao_attr(self->vao, self->vbo, 1, 2, GL_FLOAT, 0, (4 * 3) * sizeof(f32));
    
    vao_bind(self->vao);
    vbo_bind(self->ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);
}

void renderer_text(struct Renderer *self, char *text, u32 text_length, vec2s position, f32 scale, vec3s color) {
    renderer_use_shader(self, ShaderType_text);
    renderer_set_view_proj(self);
    shader_uniform_vec3(self->shader, "color", color);
    shader_uniform_mat4(self->shader, "m", glms_mat4_identity());
    
    glActiveTexture(GL_TEXTURE0);
    vao_bind(self->text.vao);
    
    for (u32 i = 0; i < text_length; ++i)
    {
        char c = text[i];
        
        struct Character ch = self->characters[c];
        
        f32 xpos = position.x + ch.bearing.x * scale;
        f32 ypos = position.y - (ch.size.y - ch.bearing.y) * scale;
        
        f32 w = ch.size.x * scale;
        f32 h = ch.size.y * scale;
        
        f32 vertices[6][4] = 
        {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f } 
        };
        
        texture_bind(ch.texture);
        vbo_bind(self->text.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        position.x += (ch.advance >> 6) * scale;
    }
}

void renderer_aabb(struct Renderer *self, AABB aabb, vec4s color,
                   mat4s model, enum FillMode fill_mode)  {
    renderer_use_shader(self, ShaderType_basic_color);
    renderer_set_view_proj(self);
    shader_uniform_mat4(self->shader, "m", model);
    shader_uniform_vec4(self->shader, "color", color);
    
    u32 indices[] = {
        1, 0, 3, 1, 3, 2, // north (-z)
        4, 5, 6, 4, 6, 7, // south (+z)
        5, 1, 2, 5, 2, 6, // east (+x)
        0, 4, 7, 0, 7, 3, // west (-x)
        2, 3, 7, 2, 7, 6, // top (+y)
        5, 4, 0, 5, 0, 1  // bottom (-y)
    };
    
    vec3s min = aabb[0], max = aabb[1];
    f32 vertices[] = {
        min.x, min.y, min.z,
        max.x, min.y, min.z,
        max.x, max.y, min.z,
        min.x, max.y, min.z,
        
        min.x, min.y, max.z,
        max.x, min.y, max.z,
        max.x, max.y, max.z,
        min.x, max.y, max.z,
    };
    
    vbo_buffer(self->vbo, vertices, 0, (8 * 3) * sizeof(f32));
    vbo_buffer(self->ibo, indices, 0, 36 * sizeof(u32));
    
    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, 0, 0);
    
    vao_bind(self->vao);
    vbo_bind(self->ibo);
    
    if (fill_mode == FillMode_line) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *) 0);
    
    if (fill_mode == FillMode_line) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}