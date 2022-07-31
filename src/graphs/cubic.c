#include "../state.h"
#include "cubic.h"

#define CUBIC_DATA_FILEPATH "cub.mdat"
#define MAX_CUBIC_GRAPHS 30u

static inline void cubic_deserialize(struct Cubic *self) {
    if (filesystem_file_exists(CUBIC_DATA_FILEPATH)) {
        FILE *file = fopen(CUBIC_DATA_FILEPATH, "r");
        fread(&self->elements, sizeof(u32), 1, file);
        fread(&self->selected_graph_index, sizeof(u32), 1, file);
        fread(&self->resolution, sizeof(u32), 1, file);
        fread(self->graphs, sizeof(struct CubicGraph), self->elements, file);
        fclose(file);
    }
}

struct Cubic *cubic_init(struct Graph *graph) {
    struct Cubic *self = malloc(sizeof(struct Cubic));
    memset(self, 0, sizeof(struct Cubic));
    self->graph = graph;
    self->graphs = calloc(MAX_CUBIC_GRAPHS, sizeof(struct Cubic));
    self->elements = 0;

    struct CubicGraph g;
    g.a = 0.001f;
    g.h = 2.0f;
    g.k = 2.0f;
    self->graphs[self->elements++] = g;
    self->resolution = state.window->size.x;
    self->graph->mesh_change_this_frame = true;

    cubic_deserialize(self);
    return self;
}

static inline void cubic_serialize(struct Cubic *self) {
    FILE *file = fopen(CUBIC_DATA_FILEPATH, "w");
    fwrite(&self->elements, sizeof(u32), 1, file);
    fwrite(&self->selected_graph_index, sizeof(u32), 1, file);
    fwrite(&self->resolution, sizeof(u32), 1, file);
    fwrite(self->graphs, sizeof(struct CubicGraph), self->elements, file);
    fclose(file);
}

void cubic_destroy(struct Cubic *self) {
    cubic_serialize(self);
    free(self);
}

void cubic_state_change(struct Cubic *self) {
    self->graph->mesh_change_this_frame = true;
}

static inline void cubic_selected_graph_update(struct CubicGraph *self,
                                               struct Cubic *cubic) {
    f32 speed = 1.0f;
    vec2s mouse_pos = state.window->mouse.position;
    if (cubic->allow_move_this_frame) {
        if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].down) {
            self->a = lerpf(
                self->a, (mouse_pos.y - self->k) / cubef(mouse_pos.x - self->h),
                0.1f);
            cubic->graph->mesh_change_this_frame = true;
        }
    } else
        cubic->allow_move_this_frame = true;

    vec2s g_velocity = glms_vec2_zero();
    if (state.window->keyboard.keys[GLFW_KEY_A].down)
        g_velocity = glms_vec2_add(g_velocity, (vec2s){{-speed, 0.0f}});
    if (state.window->keyboard.keys[GLFW_KEY_D].down)
        g_velocity = glms_vec2_add(g_velocity, (vec2s){{speed, 0.0f}});
    if (state.window->keyboard.keys[GLFW_KEY_W].down)
        g_velocity = glms_vec2_add(g_velocity, (vec2s){{0.0f, speed}});
    if (state.window->keyboard.keys[GLFW_KEY_S].down)
        g_velocity = glms_vec2_add(g_velocity, (vec2s){{0.0f, -speed}});

    if (!glms_vec2_eqv(g_velocity, glms_vec2_zero())) {
        g_velocity = glms_vec2_normalize(g_velocity);
        self->h += g_velocity.x;
        self->k += g_velocity.y;
        cubic->graph->mesh_change_this_frame = true;
    }

    self->a = clamp(self->a,
                    -((f32)cubic->resolution * state.window->size.y) /
                        (state.window->size.x * 2),
                    ((f32)cubic->resolution * state.window->size.y) /
                        (state.window->size.x * 2));
}

static inline void cubic_graph_update(struct CubicGraph *self,
                                      struct Cubic *cubic, u32 index) {
    self->pos = (vec2s){{self->h, self->k}};

    if (glms_vec2_eqve(self->pos, state.window->mouse.position,
                       POINT_SIZE / 2)) {
        if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed)
            cubic->selected_graph_index = index;
        cubic->allow_move_this_frame = false;
    }
}

void cubic_update(struct Cubic *self) {
    cubic_selected_graph_update(&self->graphs[self->selected_graph_index],
                                self);

    for (u32 i = 0; i < self->elements; ++i)
        cubic_graph_update(&self->graphs[i], self, i);

    if (state.window->keyboard.keys[GLFW_KEY_T].pressed &&
        state.window->keyboard.keys[GLFW_KEY_LEFT_CONTROL].down) {
        struct CubicGraph q = {
            .a = 0.01f,
            .h = 10.f,
            .k = -10.f,
        };
        self->selected_graph_index = self->elements;
        self->graphs[self->elements++] = q;
        self->graph->mesh_change_this_frame = true;
    }

    if (state.window->keyboard.keys[GLFW_KEY_DELETE].pressed &&
        self->elements > 0) {
        REMOVE_ARR(self->graphs, self->selected_graph_index, self->elements);
        self->elements--;
        self->graph->mesh_change_this_frame = true;
        self->selected_graph_index =
            clamp(self->selected_graph_index, 0u, self->elements - 1);
    }

    if (state.window->keyboard.keys[GLFW_KEY_N].pressed) {
        if (++self->selected_graph_index >= self->elements)
            self->selected_graph_index = 0;
    }

    self->elements = clamp(self->elements, 0u, MAX_CUBIC_GRAPHS);
}

static inline void cubic_graph_mesh(struct CubicGraph *self,
                                    struct Cubic *cubic) {
    struct Graph *graph = cubic->graph;
    for (u32 i = 0; i <= cubic->resolution; ++i) {
        f32 x = ((f32)i / (f32)cubic->resolution) * state.window->size.x -
                state.window->size.x / 2.f;
        vec2s position = (vec2s){{x, self->a * cubef(x - self->h) + self->k}};
        INSERT(graph->vertices, graph->vertex_count, position.x);
        INSERT(graph->vertices, graph->vertex_count + 1, position.y);
        INSERT(graph->indices, graph->index_count++, (graph->vertex_count / 2));
        INSERT(graph->indices, graph->index_count++,
               (graph->vertex_count / 2) + 1);
        graph->vertex_count += 2;
    }
    graph->index_count -= 2;
}

void cubic_mesh(struct Cubic *self) {
    for (u32 i = 0; i < self->elements; ++i) {
        cubic_graph_mesh(&self->graphs[i], self);
    }
}
