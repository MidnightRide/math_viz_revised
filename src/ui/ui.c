#include "../state.h"
#include "ui.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#include <string.h>

void ui_init(struct UI *self, struct Graph *graph) { self->graph = graph; }

void ui_destroy(struct UI *self) {}

void ui_update(struct UI *self) {}

static inline void ui_render_bezier_points(struct UI *self) {
    struct Bezier *bezier = self->graph->bezier;
    struct BezierGraph *selected_g =
        &bezier->graphs[bezier->selected_graph_index];
    vec2s mouse_p = state.window->mouse.position;

    if (bezier->elements > 0) {
        for (u32 i = 0; i < 4; ++i) {
            mat4s m = glms_translate(
                glms_mat4_identity(),
                (vec3s){{selected_g->p[i].x - POINT_SIZE / 2,
                         selected_g->p[i].y - POINT_SIZE / 2, 0.0f}});
            f32 alpha = (i == bezier->selected_point_index ||
                         glms_vec2_eqve(selected_g->p[i], mouse_p, 5.f))
                            ? 0.7f
                            : 0.3f;
            renderer_quad_color(&state.renderer,
                                (vec2s){{POINT_SIZE, POINT_SIZE}},
                                (vec4s){{1.0f, 1.0f, 0.0f, alpha}}, m);
        }

        for (u32 i = 0; i < bezier->elements; ++i) {
            struct BezierGraph *g = &bezier->graphs[i];
            f32 alpha = (glms_vec2_eqve(g->pos, mouse_p, POINT_SIZE / 2) ||
                         i == bezier->selected_graph_index)
                            ? 0.6f
                            : 0.2f;
            renderer_quad_color(
                &state.renderer, (vec2s){{POINT_SIZE, POINT_SIZE}},
                (vec4s){{1.0f, 1.0f, 1.0f, alpha}},
                glms_translate(glms_mat4_identity(),
                               (vec3s){{g->pos.x - POINT_SIZE / 2,
                                        g->pos.y - POINT_SIZE / 2, 0.f}}));
        }
    }
}

static inline void ui_render_linear_points(struct UI *self) {
    struct Linear *linear = self->graph->linear;
    vec2s mouse_p = state.window->mouse.position;

    for (u32 i = 0; i < linear->elements; ++i) {
        struct LinearGraph *g = &linear->graphs[i];

        mat4s m = glms_translate(glms_mat4_identity(),
                                 (vec3s){{g->pos.x - POINT_SIZE / 2,
                                          g->pos.y - POINT_SIZE / 2, 0.0f}});
        f32 alpha = (glms_vec2_eqve(g->pos, mouse_p, POINT_SIZE / 2) ||
                     i == linear->selected_graph_index)
                        ? 0.6f
                        : 0.2f;
        renderer_quad_color(&state.renderer, (vec2s){{POINT_SIZE, POINT_SIZE}},
                            (vec4s){{1.0f, 1.0f, 1.0f, alpha}}, m);
    }
}

static inline void ui_render_quadratic_points(struct UI *self) {
    struct Quadratic *quadratic = self->graph->quadratic;
    vec2s mouse_p = state.window->mouse.position;

    for (u32 i = 0; i < quadratic->elements; ++i) {
        struct QuadraticGraph *g = &quadratic->graphs[i];

        mat4s m = glms_translate(glms_mat4_identity(),
                                 (vec3s){{g->pos.x - POINT_SIZE / 2,
                                          g->pos.y - POINT_SIZE / 2, 0.0f}});
        f32 alpha = (glms_vec2_eqve(g->pos, mouse_p, POINT_SIZE / 2) ||
                     i == quadratic->selected_graph_index)
                        ? 0.6f
                        : 0.2f;
        renderer_quad_color(&state.renderer, (vec2s){{POINT_SIZE, POINT_SIZE}},
                            (vec4s){{1.0f, 1.0f, 1.0f, alpha}}, m);
    }
}

static inline void ui_render_cubic_points(struct UI *self) {
    struct Cubic *cubic = self->graph->cubic;
    vec2s mouse_p = state.window->mouse.position;

    for (u32 i = 0; i < cubic->elements; ++i) {
        struct CubicGraph *g = &cubic->graphs[i];

        mat4s m = glms_translate(glms_mat4_identity(),
                                 (vec3s){{g->pos.x - POINT_SIZE / 2,
                                          g->pos.y - POINT_SIZE / 2, 0.0f}});
        f32 alpha = (glms_vec2_eqve(g->pos, mouse_p, POINT_SIZE / 2) ||
                     i == cubic->selected_graph_index)
                        ? 0.6f
                        : 0.2f;
        renderer_quad_color(&state.renderer, (vec2s){{POINT_SIZE, POINT_SIZE}},
                            (vec4s){{1.0f, 1.0f, 1.0f, alpha}}, m);
    }
}

static inline void ui_render_label(struct UI *self, char *name, vec2s position,
                                   b32 highlighted) {
    u32 name_length = strlen(name);
    f32 margin = 5.0f;
    f32 line_width = 2.0f;

    vec4s background_color =
        (vec4s){{0.4f, 0.4f, 0.4f, highlighted ? 1.0f : 0.2f}};
    vec4s line_color = (vec4s){{0.3f, 0.3f, 0.3f, 1.0f}};
    vec2s button_size = (vec2s){{name_length * 9.f + margin, 16.f + margin}};

    mat4s m = glms_translate(
        glms_mat4_identity(),
        (vec3s){{position.x - margin, position.y - margin, 0.0f}});
    renderer_quad_color(&state.renderer,
                        (vec2s){{button_size.x, button_size.y}},
                        background_color, m);
    renderer_text(&state.renderer, name, name_length, position, 0.3f,
                  (vec3s){{1.0f, 1.0f, 1.0f}});

    // left line
    renderer_quad_color(
        &state.renderer, (vec2s){{line_width, button_size.y + line_width * 2}},
        line_color,
        glms_translate(m, (vec3s){{-line_width, -line_width, 0.0f}}));

    // right line
    renderer_quad_color(
        &state.renderer, (vec2s){{line_width, button_size.y + line_width * 2}},
        line_color,
        glms_translate(m, (vec3s){{button_size.x, -line_width, 0.0f}}));

    // bottom line
    renderer_quad_color(
        &state.renderer,
        (vec2s){{button_size.x + 2.0f * line_width, line_width}}, line_color,
        glms_translate(m, (vec3s){{-line_width, -line_width, 0.0f}}));

    // top line
    renderer_quad_color(
        &state.renderer,
        (vec2s){{button_size.x + 2.0f * line_width, line_width}}, line_color,
        glms_translate(m, (vec3s){{-line_width, button_size.y, 0.0f}}));
}

static inline void ui_render_slider(struct UI *self, vec2s position, f32 value,
                                    f32 min, f32 max) {
    f32 margin = 5.0f;
    f32 line_width = 2.0f;

    vec4s background_color = (vec4s){{0.4f, 0.4f, 0.4f, 0.2f}};
    vec4s line_color = (vec4s){{0.3f, 0.3f, 0.3f, 1.0f}};
    vec2s button_size = (vec2s){{60.f + margin, 16.f + margin}};

    mat4s m = glms_translate(
        glms_mat4_identity(),
        (vec3s){{position.x - margin, position.y - margin, 0.0f}});

    f32 pcent = clamp((value - min) / (max - min), 0.0f, 1.0f);

    char name[20];
    snprintf(name, 20, "%*.*f", 3, 1, value);
    if (strlen(name) > 4)
        name[5] = '\0';

    renderer_quad_color(&state.renderer,
                        (vec2s){{button_size.x, button_size.y}},
                        background_color, m);
    renderer_text(&state.renderer, name, strlen(name), position, 0.3f,
                  (vec3s){{1.0f, 1.0f, 1.0f}});

    // left line
    renderer_quad_color(
        &state.renderer, (vec2s){{line_width, button_size.y + line_width * 2}},
        line_color,
        glms_translate(m, (vec3s){{-line_width, -line_width, 0.0f}}));

    // right line
    renderer_quad_color(
        &state.renderer, (vec2s){{line_width, button_size.y + line_width * 2}},
        line_color,
        glms_translate(m, (vec3s){{button_size.x, -line_width, 0.0f}}));

    // bottom line
    renderer_quad_color(
        &state.renderer,
        (vec2s){{button_size.x + 2.0f * line_width, line_width}}, line_color,
        glms_translate(m, (vec3s){{-line_width, -line_width, 0.0f}}));

    // top line
    renderer_quad_color(
        &state.renderer,
        (vec2s){{button_size.x + 2.0f * line_width, line_width}}, line_color,
        glms_translate(m, (vec3s){{-line_width, button_size.y, 0.0f}}));

    // middle line
    m = glms_translate(
        glms_mat4_identity(),
        (vec3s){{position.x - margin, position.y - margin, 0.0f}});
    renderer_quad_color(
        &state.renderer, (vec2s){{line_width, button_size.y + line_width * 2}},
        line_color,
        glms_translate(
            m, (vec3s){{-line_width + pcent * (button_size.x + line_width),
                        -line_width, 0.0f}}));
}

static inline void ui_render_bezier(struct UI *self) {
    ui_render_bezier_points(self);
}

static inline void ui_render_quadratic(struct UI *self) {
    struct Quadratic *quadratic = self->graph->quadratic;
    ui_render_quadratic_points(self);
    if (quadratic->elements > 0)
        ui_render_slider(self,
                         (vec2s){{-state.window->size.x / 2 + 12.f,
                                  -state.window->size.y / 2 + 12.f}},
                         quadratic->graphs[quadratic->selected_graph_index].a *
                             100,
                         -1.f, 1.f);
}

static inline void ui_render_linear(struct UI *self) {
    struct Linear *linear = self->graph->linear;
    ui_render_linear_points(self);
    if (linear->elements > 0)
        ui_render_slider(self,
                         (vec2s){{-state.window->size.x / 2 + 12.f,
                                  -state.window->size.y / 2 + 12.f}},
                         linear->graphs[linear->selected_graph_index].m, -2.f,
                         2.f);
}

static inline void ui_render_cubic(struct UI *self) {
    struct Cubic *cubic = self->graph->cubic;
    ui_render_cubic_points(self);
    if (cubic->elements > 0)
        ui_render_slider(self,
                         (vec2s){{-state.window->size.x / 2 + 12.f,
                                  -state.window->size.y / 2 + 12.f}},
                         cubic->graphs[cubic->selected_graph_index].a * 10000.f,
                         -3.f, 3.f);
}

void ui_render(struct UI *self) {
    switch (self->graph->state) {
    case GraphState_bezier: {
        ui_render_bezier(self);
    } break;
    case GraphState_linear: {
        ui_render_linear(self);
    } break;
    case GraphState_cubic: {
        ui_render_cubic(self);
    } break;
    default:
    case GraphState_quadratic: {
        ui_render_quadratic(self);
    } break;
    }

    ui_render_label(self, "Bezier",
                    (vec2s){{state.window->size.x / 2 - 61.f,
                             -state.window->size.y / 2 + 12.f}},
                    self->graph->state == GraphState_bezier);
    ui_render_label(self, "Quadratic",
                    (vec2s){{state.window->size.x / 2 - 157.f,
                             -state.window->size.y / 2 + 12.f}},
                    self->graph->state == GraphState_quadratic);
    ui_render_label(self, "Linear",
                    (vec2s){{state.window->size.x / 2 - 226.f,
                             -state.window->size.y / 2 + 12.f}},
                    self->graph->state == GraphState_linear);

    char temp[50];
    snprintf(temp, 50, "%f:%f",
             state.window->size.x / 2 - state.window->mouse.position.x,
             state.window->mouse.position.y);
    ui_render_label(self, temp,
                    (vec2s){{-state.window->size.x / 2 + 13.f,
                             state.window->size.y / 2 - 27.f}},
                    false);
}
