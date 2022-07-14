#include "bezier.h"
#include "../state.h"

#define BEZIER_DATA_FILEPATH "bez.mdat"

#define MAX_BEZIER_GRAPHS 512u

static inline void bezier_deserialize(struct Bezier *self) {
    char cwd[100];
    filesystem_get_dir(cwd, sizeof(cwd));
    
    char buffer[50];
    snprintf(buffer, 50, "%s/Documents/MathVizRevised/", getenv("USERPROFILE"));
    
    if (!filesystem_dir_exists(buffer)) filesystem_create_dir(buffer);
    
    filesystem_change_dir(buffer);
    if (filesystem_file_exists(BEZIER_DATA_FILEPATH)) {
        FILE *file = fopen(BEZIER_DATA_FILEPATH, "r");
        fread(&self->elements, sizeof(u32), 1, file);
        fread(&self->selected_graph_index, sizeof(u32), 1, file);
        fread(&self->selected_point_index, sizeof(u32), 1, file);
        fread(&self->resolution, sizeof(u32), 1, file);
        fread(self->graphs, sizeof(struct BezierGraph), self->elements, file);
        fclose(file);
    }
    filesystem_change_dir(cwd);
}

struct Bezier *bezier_init(struct Graph *graph) {
    struct Bezier *self = malloc(sizeof(struct Bezier));
    memset(self, 0, sizeof(struct Bezier));
    self->graph = graph;
    self->graphs = calloc(MAX_GRAPH_COUNT, sizeof(struct BezierGraph));
    self->elements = 0;
    
    struct BezierGraph g;
    g.p[0] = (vec2s) {{-100.0f, -100.f}};
    g.p[1] = (vec2s) {{-50.0f, -200.f}};
    g.p[2] = (vec2s) {{100.0f, 100.f}};
    g.p[3] = (vec2s) {{-20.0f, 300.f}};
    g.color = (vec4s) {{ 1.0f, 1.0f, 1.0f, 1.0f }};
    self->graphs[self->elements++] = g;
    
    struct BezierGraph g1;
    g1.p[0] = (vec2s) {{-100.0f, -200.f}};
    g1.p[1] = (vec2s) {{-50.0f, -20.f}};
    g1.p[2] = (vec2s) {{100.0f, 200.f}};
    g1.p[3] = (vec2s) {{-20.0f, 20.f}};
    g1.color = (vec4s) {{ 1.0f, 1.0f, 1.0f, 1.0f }};
    self->graphs[self->elements++] = g1;
    self->graph->mesh_change_this_frame = true;
    self->resolution = 30;
    
    bezier_deserialize(self);
    
    return self;
}

static inline void bezier_serialize(struct Bezier *self) {
    char cwd[100];
    filesystem_get_dir(cwd, 100);
    char buffer[50];
    snprintf(buffer, 50, "%s/Documents/MathVizRevised/", getenv("USERPROFILE"));
    
    filesystem_change_dir(buffer);
    FILE *file = fopen(BEZIER_DATA_FILEPATH, "w");
    fwrite(&self->elements, sizeof(u32), 1, file);
    fwrite(&self->selected_graph_index, sizeof(u32), 1, file);
    fwrite(&self->selected_point_index, sizeof(u32), 1, file);
    fwrite(&self->resolution, sizeof(u32), 1, file);
    fwrite(self->graphs, sizeof(struct BezierGraph), self->elements, file);
    fclose(file);
    fclose(file);
    filesystem_change_dir(cwd);
}

void bezier_destroy(struct Bezier *self) {
    bezier_serialize(self);
    free(self);
}

void bezier_state_change(struct Bezier *self) {
    self->graph->mesh_change_this_frame = true;
}

static inline void bezier_graph_update(struct BezierGraph *self, struct Bezier *bezier, u32 index) {
    self->pos = bezier4(self->p[0], self->p[1], self->p[2], self->p[3], 0.5f);
    
    if (glms_vec2_eqve(self->pos, state.window->mouse.position, POINT_SIZE/2)) {
        if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed) bezier->selected_graph_index = index;
        bezier->allow_move_this_frame = false;
    }
}

static inline void selected_graph_update(struct BezierGraph *self, struct Bezier *bezier) {
    for (u32 i = 0; i < 4; ++i) {
        if (glms_vec2_eqve(self->p[i], state.window->mouse.position, POINT_SIZE/2) &&
            state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed) {
            bezier->selected_point_index = i;
        }
    }
    
    if (bezier->allow_move_this_frame) {
        if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].down) {
            self->p[bezier->selected_point_index] = glms_vec2_lerp(self->p[bezier->selected_point_index], state.window->mouse.position, 0.1f);
            bezier->graph->mesh_change_this_frame = true;
        }
    } else bezier->allow_move_this_frame = true;
}

void bezier_update(struct Bezier *self) {
    selected_graph_update(&self->graphs[self->selected_graph_index], self);
    
    for (u32 i = 0; i < self->elements; ++i) {
        bezier_graph_update(&self->graphs[i], self, i);
    }
    
    if (state.window->keyboard.keys[GLFW_KEY_MINUS].down) {
        self->resolution--;
        self->graph->mesh_change_this_frame = true;
    }
    
    if (state.window->keyboard.keys[GLFW_KEY_EQUAL].down) {
        self->resolution++;
        self->graph->mesh_change_this_frame = true;
    }
    
    if (state.window->keyboard.keys[GLFW_KEY_N].pressed) {
        if (++self->selected_graph_index >= self->elements) self->selected_graph_index = 0;
    }
    
    if (state.window->keyboard.keys[GLFW_KEY_T].pressed && state.window->keyboard.keys[GLFW_KEY_LEFT_CONTROL].down) {
        struct BezierGraph g;
        g.p[0] = (vec2s) {{-100.0f, -100.f}};
        g.p[1] = (vec2s) {{-50.0f, -200.f}};
        g.p[2] = (vec2s) {{100.0f, 100.f}};
        g.p[3] = (vec2s) {{-20.0f, 300.f}};
        g.color = (vec4s) {{ 1.0f, 1.0f, 1.0f, 1.0f }};
        self->selected_graph_index = self->elements;
        self->graphs[self->elements++] = g;
        self->graph->mesh_change_this_frame = true;
    }
    
    if (state.window->keyboard.keys[GLFW_KEY_DELETE].pressed && self->elements > 0) {
        REMOVE_ARR(self->graphs, self->selected_graph_index, self->elements);
        self->elements--;
        self->graph->mesh_change_this_frame = true;
        self->selected_graph_index = clamp(self->selected_graph_index, 0u, self->elements-1);
    }
    
    self->resolution = clamp(self->resolution, 10u, 50u);
    
    self->elements = clamp(self->elements, 0u, MAX_BEZIER_GRAPHS);
}

static inline void bezier_graph_mesh(struct BezierGraph *self, struct Bezier *bezier) {
    struct Graph *graph = bezier->graph;
    for (u32 w = 0; w <= bezier->resolution; ++w)  {
        f32 t = (f32)w/(f32)bezier->resolution;
        vec2s position = bezier4(self->p[0], self->p[1], self->p[2], self->p[3], t);
        INSERT(graph->vertices, graph->vertex_count, position.x);
        INSERT(graph->vertices, graph->vertex_count + 1, position.y);
        INSERT(graph->indices, graph->index_count++, (graph->vertex_count/2));
        INSERT(graph->indices, graph->index_count++, (graph->vertex_count/2)+1);
        graph->vertex_count += 2;
    }
    graph->index_count-=2;
}

void bezier_mesh(struct Bezier *self) {
    for (u32 i = 0; i < self->elements; ++i) {
        bezier_graph_mesh(&self->graphs[i], self);
    }
}

