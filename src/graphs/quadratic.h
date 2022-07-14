/* date = July 5th 2022 2:39 pm */

#ifndef QUADRATIC_GRAPH_H
#define QUADRATIC_GRAPH_H

#include "../util/util.h"
#include "graph.h"

#define MINIMUM_STEP 0.0001f
#define LINE_STEP 0.05f

#define POINT_SIZE 10.f

struct QuadraticGraph {
    f32 a, h, k;
    vec2s pos;
};

struct Quadratic {
    struct Graph *graph;
    
    struct QuadraticGraph *graphs;
    u32 elements;
    u32 selected_graph_index; 
    
    u32 resolution;
    
    b8 allow_move_this_frame;
};

struct Quadratic *quadratic_init(struct Graph *graph);
void quadratic_destroy(struct Quadratic *self);
void quadratic_state_change(struct Quadratic *self);
void quadratic_update(struct Quadratic *self);
void quadratic_mesh(struct Quadratic *self);

#endif //QUADRATIC_GRAPH_H
