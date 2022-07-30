#ifndef CUBIC_H
#define CUBIC_H

#include "../util/util.h"

struct CubicGraph {
    f32 a, h, k;
    vec2s pos;
};

struct Cubic {
    struct Graph *graph;

    struct CubicGraph *graphs;
    u32 elements;
    u32 selected_graph_index;

    u32 resolution;

    b8 allow_move_this_frame;
};

struct Cubic *cubic_init(struct Graph *graph);
void cubic_destroy(struct Cubic *self);
void cubic_state_change(struct Cubic *self);
void cubic_update(struct Cubic *self);
void cubic_mesh(struct Cubic *self);

#endif
