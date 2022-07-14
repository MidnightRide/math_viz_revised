#include "graph.h"
#include "../state.h"

#define GRAPH_DATA_FILEPATH "graphs.mdat"

static inline void graph_deserialize(struct Graph *self) {
    char cwd[100];
    filesystem_get_dir(cwd, sizeof(cwd));
    
    char buffer[50];
    snprintf(buffer, 50, "%s/Documents/MathVizRevised/", getenv("USERPROFILE"));
    
    if (!filesystem_dir_exists(buffer)) filesystem_create_dir(buffer);
    
    filesystem_change_dir(buffer);
    if (filesystem_file_exists(GRAPH_DATA_FILEPATH)) {
        FILE *file = fopen(GRAPH_DATA_FILEPATH, "r");
        fread(&self->state, sizeof(enum GraphState), 1, file);
        fclose(file);
    }
    filesystem_change_dir(cwd);
}

void graph_init(struct Graph *self) { 
    self->bezier = bezier_init(self);
    self->linear = linear_init(self);
    self->quadratic = quadratic_init(self);
    
    self->state = GraphState_quadratic;
    
    self->vao = vao_create();
    self->vbo = vbo_create(GL_ARRAY_BUFFER, false);
    self->ibo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, false);
    
    graph_deserialize(self);
}

static inline void graph_serialize(struct Graph *self) {
    char cwd[100];
    filesystem_get_dir(cwd, 100);
    char buffer[50];
    snprintf(buffer, 50, "%s/Documents/MathVizRevised/", getenv("USERPROFILE"));
    
    filesystem_change_dir(buffer);
    FILE *file = fopen(GRAPH_DATA_FILEPATH, "w");
    fwrite(&self->state, sizeof(enum GraphState), 1, file);
    fclose(file);
    filesystem_change_dir(cwd);
}

void graph_destroy(struct Graph *self) {
    graph_serialize(self);
    vao_destroy(self->vao);
    vbo_destroy(self->vbo);
    vbo_destroy(self->ibo);
    bezier_destroy(self->bezier);
    linear_destroy(self->linear);
    quadratic_destroy(self->quadratic);
}

void graph_resized(struct Graph *self) {
    self->mesh_change_this_frame = true;
}

void graph_mesh_prepare(struct Graph *self)
{
    self->vertex_count = 0;
    self->index_count = 0;
}

static inline void graph_handle_inputs(struct Graph *self) {
    struct Keyboard keyboard = state.window->keyboard;
    
    if (keyboard.keys[GLFW_KEY_3].pressed) {
        self->state = GraphState_bezier;
        bezier_state_change(self->bezier);
    }
    else if (keyboard.keys[GLFW_KEY_1].pressed) {
        self->state = GraphState_linear;
        linear_state_change(self->linear);
    }
    else if (keyboard.keys[GLFW_KEY_2].pressed) {
        self->state = GraphState_quadratic;
        quadratic_state_change(self->quadratic);
    }
}

void graph_update(struct Graph *self)
{
    graph_handle_inputs(self);
    
    switch (self->state) {
        case GraphState_bezier: {
            bezier_update(self->bezier); 
        } break;
        case GraphState_linear: {
            linear_update(self->linear); 
        } break;
        case GraphState_quadratic: {
            quadratic_update(self->quadratic); 
        } break;
        default: bezier_update(self->bezier); break;
    }
}

void graph_render(struct Graph *self) {
    glEnable(GL_MULTISAMPLE);
    
    renderer_use_shader(&state.renderer, ShaderType_basic_color);
    renderer_set_view_proj(&state.renderer);
    
    shader_uniform_mat4(state.renderer.shaders[ShaderType_basic_color], "m", glms_mat4_identity());
    shader_uniform_vec4(state.renderer.shaders[ShaderType_basic_color], "color", (vec4s){{ 1.f, 0.15f, 0.15f, 1.f }});
    
    if (self->mesh_change_this_frame) {
        graph_mesh_prepare(self);
        switch (self->state) {
            case GraphState_bezier: {
                bezier_mesh(self->bezier); 
            } break;
            case GraphState_linear: {
                linear_mesh(self->linear); 
            } break;
            case GraphState_quadratic: {
                quadratic_mesh(self->quadratic); 
            } break;
            default: bezier_mesh(self->bezier); break;
        }
        self->mesh_change_this_frame = false;
    }
    
    vbo_buffer(self->vbo, self->vertices, 0, sizeof(f32) * self->vertex_count);
    
    vbo_buffer(self->ibo, self->indices, 0, sizeof(u32) * self->index_count);
    
    vao_attr(self->vao, self->vbo, 0, 2, GL_FLOAT, sizeof(f32)*2, 0);
    
    vao_bind(self->vao);
    vbo_bind(self->ibo);
    
    glDrawElements(GL_LINES, self->index_count, GL_UNSIGNED_INT, (void *)0);
    
    glDisable(GL_MULTISAMPLE);
}