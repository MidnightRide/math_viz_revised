#include "../state.h"
#include "quadratic.h"

#define QUADRATIC_DATA_FILEPATH "quad.mdat"
#define MAX_QUADRATIC_GRAPHS 30u

static inline void quadratic_deserialize(struct Quadratic *self) {
    char cwd[100];
    filesystem_get_dir(cwd, sizeof(cwd));

    char buffer[50];
    snprintf(buffer, 50, "%s/Documents/MathVizRevised/", getenv("USERPROFILE"));

    if (!filesystem_dir_exists(buffer))
        filesystem_create_dir(buffer);

    filesystem_change_dir(buffer);
    if (filesystem_file_exists(QUADRATIC_DATA_FILEPATH)) {
        FILE *file = fopen(QUADRATIC_DATA_FILEPATH, "r");
        fread(&self->elements, sizeof(u32), 1, file);
        fread(&self->selected_graph_index, sizeof(u32), 1, file);
        fread(&self->resolution, sizeof(u32), 1, file);
        fread(self->graphs, sizeof(struct QuadraticGraph), self->elements,
              file);
        fclose(file);
    }
    filesystem_change_dir(cwd);
}

struct Quadratic *quadratic_init(struct Graph *graph) {
    struct Quadratic *self = malloc(sizeof(struct Quadratic));
    memset(self, 0, sizeof(struct Quadratic));
    self->graph = graph;
    self->graphs = calloc(MAX_GRAPH_COUNT, sizeof(struct QuadraticGraph));
    self->elements = 0;

    struct QuadraticGraph g;
    g.a = 2.0f;
    g.h = 2.0f;
    g.k = 2.0f;
    self->graphs[self->elements++] = g;

    struct QuadraticGraph g1;
    g1.a = 1.0f;
    g1.h = 20.0f;
    g1.k = 50.0f;
    self->graphs[self->elements++] = g1;
    self->graph->mesh_change_this_frame = true;
    self->resolution = 500;

    quadratic_deserialize(self);
    return self;
}

static inline void quadratic_serialize(struct Quadratic *self) {
    char cwd[100];
    filesystem_get_dir(cwd, 100);
    char buffer[50];
    snprintf(buffer, 50, "%s/Documents/MathVizRevised/", getenv("USERPROFILE"));

    filesystem_change_dir(buffer);
    FILE *file = fopen(QUADRATIC_DATA_FILEPATH, "w");
    fwrite(&self->elements, sizeof(u32), 1, file);
    fwrite(&self->selected_graph_index, sizeof(u32), 1, file);
    fwrite(&self->resolution, sizeof(u32), 1, file);
    fwrite(self->graphs, sizeof(struct QuadraticGraph), self->elements, file);
    fclose(file);
    filesystem_change_dir(cwd);
}

void quadratic_destroy(struct Quadratic *self) {
    quadratic_serialize(self);
    free(self);
}

void quadratic_state_change(struct Quadratic *self) {
    self->graph->mesh_change_this_frame = true;
}

void quadratic_selected_graph_update(struct QuadraticGraph *self,
                                     struct Quadratic *quadratic) {
    f32 speed = 1.0f;
    vec2s mouse_pos = state.window->mouse.position;
    if (quadratic->allow_move_this_frame) {
        if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].down) {
            self->a = lerpf(
                self->a,
                (mouse_pos.y - self->k) / squaref(mouse_pos.x - self->h), 0.1f);
            quadratic->graph->mesh_change_this_frame = true;
        }
    } else
        quadratic->allow_move_this_frame = true;

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
        quadratic->graph->mesh_change_this_frame = true;
    }

    self->a = clamp(self->a,
                    -((f32)quadratic->resolution * state.window->size.y) /
                        (state.window->size.x * 2),
                    ((f32)quadratic->resolution * state.window->size.y) /
                        (state.window->size.x * 2));
}

static inline void quadratic_graph_update(struct QuadraticGraph *self,
                                          struct Quadratic *quadratic,
                                          u32 index) {
    self->pos = (vec2s){{self->h, self->k}};

    if (glms_vec2_eqve(self->pos, state.window->mouse.position,
                       POINT_SIZE / 2)) {
        if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed)
            quadratic->selected_graph_index = index;
        quadratic->allow_move_this_frame = false;
    }
}

void quadratic_update(struct Quadratic *self) {
    quadratic_selected_graph_update(&self->graphs[self->selected_graph_index],
                                    self);

    for (u32 i = 0; i < self->elements; ++i) {
        quadratic_graph_update(&self->graphs[i], self, i);
    }

    if (state.window->keyboard.keys[GLFW_KEY_T].pressed &&
        state.window->keyboard.keys[GLFW_KEY_LEFT_CONTROL].down) {
        struct QuadraticGraph q = {
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

    self->elements = clamp(self->elements, 0u, MAX_QUADRATIC_GRAPHS);
}

static inline void quadratic_graph_mesh(struct QuadraticGraph *self,
                                        struct Quadratic *quadratic) {
    struct Graph *graph = quadratic->graph;
    for (u32 i = 0; i <= quadratic->resolution; ++i) {
        f32 x = ((f32)i / (f32)quadratic->resolution) * state.window->size.x -
                state.window->size.x / 2.f;
        vec2s position = (vec2s){{x, self->a * squaref(x - self->h) + self->k}};
        INSERT(graph->vertices, graph->vertex_count, position.x);
        INSERT(graph->vertices, graph->vertex_count + 1, position.y);
        INSERT(graph->indices, graph->index_count++, (graph->vertex_count / 2));
        INSERT(graph->indices, graph->index_count++,
               (graph->vertex_count / 2) + 1);
        graph->vertex_count += 2;
    }
    graph->index_count -= 2;
}

void quadratic_mesh(struct Quadratic *self) {
    for (u32 i = 0; i < self->elements; ++i) {
        quadratic_graph_mesh(&self->graphs[i], self);
    }
}
