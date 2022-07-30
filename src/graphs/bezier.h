/* date = July 5th 2022 2:39 pm */

#ifndef BEZIER_GRAPH_H
#define BEZIER_GRAPH_H

#include "../util/util.h"
#include "../io/filesystem.h"
#include "graph.h"

#define MINIMUM_STEP 0.0001f
#define LINE_STEP 0.05f

#define POINT_SIZE 10.f

struct BezierGraph {
    vec2s p[4], pos;
    vec4s color;
};

struct Bezier {
    struct Graph *graph;
    
    struct BezierGraph *graphs;
    u32 elements;
    u32 selected_graph_index; 
    u32 selected_point_index;
    
    u32 resolution;
    
    b8 allow_move_this_frame;
};

struct Bezier *bezier_init(struct Graph *graph);
void bezier_destroy(struct Bezier *self);
void bezier_state_change(struct Bezier *self);
void bezier_update(struct Bezier *self);
void bezier_mesh(struct Bezier *self);

#endif //BEZIER_GRAPH_H
