/* date = July 5th 2022 2:39 pm */

#ifndef LINEAR_GRAPH_H
#define LINEAR_GRAPH_H

#include "../util/util.h"
#include "graph.h"

struct LinearGraph {
    f32 m, b;
    vec2s pos, target_pos;
};

struct Linear {
    struct Graph *graph;
    
    struct LinearGraph *graphs;
    u32 elements;
    u32 selected_graph_index;
    
    b8 allow_move_this_frame;
};

struct Linear *linear_init(struct Graph *graph);
void linear_destroy(struct Linear *self);
void linear_state_change(struct Linear *self);
void linear_update(struct Linear *self);
void linear_mesh(struct Linear *self);

#endif //LINEAR_GRAPH_H
