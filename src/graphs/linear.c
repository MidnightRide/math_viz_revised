#include "linear.h"
#include "../state.h"

#define LINEAR_DATA_FILEPATH "lin.mdat"

#define MAX_LINEAR_GRAPHS 2048u

static inline void linear_deserialize(struct Linear *self) {
    char cwd[100];
    filesystem_get_dir(cwd, sizeof(cwd));
    
    char buffer[50];
    snprintf(buffer, 50, "%s/Documents/MathVizRevised/", getenv("USERPROFILE"));
    
    if (!filesystem_dir_exists(buffer)) filesystem_create_dir(buffer);
    
    filesystem_change_dir(buffer);
    if (filesystem_file_exists(LINEAR_DATA_FILEPATH)) {
        FILE *file = fopen(LINEAR_DATA_FILEPATH, "r");
        fread(&self->elements, sizeof(u32), 1, file);
        fread(&self->selected_graph_index, sizeof(u32), 1, file);
        fread(self->graphs, sizeof(struct LinearGraph), self->elements, file);
        fclose(file);
    }
    filesystem_change_dir(cwd);
}

struct Linear *linear_init(struct Graph *graph) {
    struct Linear *self = malloc(sizeof(struct Linear));
    memset(self, 0, sizeof(struct Linear));
    self->graph = graph;
    self->graphs = calloc(MAX_GRAPH_COUNT, sizeof(struct LinearGraph));
    self->elements = 0;
    
    struct LinearGraph l;
    l.m = 2.0f;
    l.b = 10.f;
    self->graphs[self->elements++] = l;
    
    struct LinearGraph l1;
    l1.m = 5.0f;
    l1.b = 10.f;
    self->graphs[self->elements++] = l1;
    self->graph->mesh_change_this_frame = true;
    self->allow_move_this_frame = true;
    
    linear_deserialize(self);
    return self;
}

static inline void linear_serialize(struct Linear *self) {
    char cwd[100];
    filesystem_get_dir(cwd, 100);
    char buffer[50];
    snprintf(buffer, 50, "%s/Documents/MathVizRevised/", getenv("USERPROFILE"));
    
    filesystem_change_dir(buffer);
    FILE *file = fopen(LINEAR_DATA_FILEPATH, "w");
    fwrite(&self->elements, sizeof(u32), 1, file);
    fwrite(&self->selected_graph_index, sizeof(u32), 1, file);
    fwrite(self->graphs, sizeof(struct LinearGraph), self->elements, file);
    fclose(file);
    fclose(file);
    filesystem_change_dir(cwd);
}

void linear_destroy(struct Linear *self) {
    linear_serialize(self);
    free(self);
}

void linear_state_change(struct Linear *self) {
    self->graph->mesh_change_this_frame = true;
}

static inline void linear_selected_graph_update(struct LinearGraph *self, struct Linear *linear) {
    f32 speed = 5.0f;
    vec2s mouse_pos = state.window->mouse.position;
    if (linear->allow_move_this_frame) {
        // check mouse position is on the right side of the screen
        if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].down && mouse_pos.x > 0) {
            self->m = lerpf(self->m, (mouse_pos.y - self->b)/mouse_pos.x, 0.1f);
            linear->graph->mesh_change_this_frame = true;
        }
        else if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].down) {
            self->m = lerpf(self->m, mouse_pos.y > 0 ? state.window->size.y/2 : -state.window->size.y/2, 0.1f);
            linear->graph->mesh_change_this_frame = true;
        }
    } else linear->allow_move_this_frame = true;
    
    vec2s g_velocity = glms_vec2_zero();
    if (state.window->keyboard.keys[GLFW_KEY_W].down)
        g_velocity = glms_vec2_add(g_velocity, (vec2s){{ 0.0f, speed }});
    if (state.window->keyboard.keys[GLFW_KEY_S].down) 
        g_velocity = glms_vec2_add(g_velocity, (vec2s){{ 0.0f, -speed }});
    
    if (!glms_vec2_eqv(g_velocity, glms_vec2_zero())) {
        g_velocity = glms_vec2_normalize(g_velocity);
        self->b += g_velocity.y;
        linear->graph->mesh_change_this_frame = true;
    }
    
    self->m = clamp(self->m, -state.window->size.y/2, state.window->size.y/2);
}

static inline void linear_graph_update(struct LinearGraph *self, struct Linear *linear, u32 index) {
    f32 x_point_offset = 50.f;
    
    self->target_pos = (vec2s) {{ x_point_offset, self->m*x_point_offset+self->b }};
    
    if (!glms_vec2_eqv(self->target_pos, self->pos) && (self->m*x_point_offset+self->b < state.window->size.y/2 && self->m*x_point_offset+self->b > -state.window->size.y/2)) {
        self->pos = self->target_pos;
    } else if (!glms_vec2_eqv(self->target_pos, self->pos)) {
        self->pos.x = lerpf(self->pos.x, 0.0f, 0.1f);
        self->pos.y = lerpf(self->pos.y, 0.0f, 0.1f);
    }
    
    if (glms_vec2_eqve(self->pos, state.window->mouse.position, POINT_SIZE/2)) {
        if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed) linear->selected_graph_index = index;
        linear->allow_move_this_frame = false;
    }
}

void linear_update(struct Linear *self) {
    linear_selected_graph_update(&self->graphs[self->selected_graph_index], self);
    
    for (u32 i = 0; i < self->elements; ++i) {
        linear_graph_update(&self->graphs[i], self, i);
    }
    
    if (state.window->keyboard.keys[GLFW_KEY_T].pressed && state.window->keyboard.keys[GLFW_KEY_LEFT_CONTROL].down) {
        struct LinearGraph l = {
            .m = 2.0f,
            .b = 10.f,
        };
        self->graphs[self->elements++] = l;
        self->graph->mesh_change_this_frame = true;
    }
    
    if (state.window->keyboard.keys[GLFW_KEY_DELETE].pressed && self->elements > 0) {
        REMOVE_ARR(self->graphs, self->selected_graph_index, self->elements);
        self->elements--;
        self->graph->mesh_change_this_frame = true;
        self->selected_graph_index = clamp(self->selected_graph_index, 0u, self->elements-1);
    }
    
    if (state.window->keyboard.keys[GLFW_KEY_N].pressed) {
        if (++self->selected_graph_index >= self->elements) self->selected_graph_index = 0;
    }
    
    self->elements = clamp(self->elements, 0u, MAX_LINEAR_GRAPHS);
}

void linear_graph_mesh(struct LinearGraph *self, struct Linear *linear) {
    struct Graph *graph = linear->graph;
    INSERT(graph->vertices, graph->vertex_count++, -state.window->size.x/2);
    INSERT(graph->vertices, graph->vertex_count++, self->m*(-state.window->size.x/2) + self->b);
    INSERT(graph->vertices, graph->vertex_count++, state.window->size.x/2);
    INSERT(graph->vertices, graph->vertex_count++, self->m*(state.window->size.x/2) + self->b);
    INSERT(graph->indices, graph->index_count, graph->index_count);
    INSERT(graph->indices, graph->index_count + 1, graph->index_count + 1);
    graph->index_count += 2;
}

void linear_mesh(struct Linear *self) {
    for (u32 i = 0; i < self->elements; ++i) {
        linear_graph_mesh(&self->graphs[i], self);
    }
}
