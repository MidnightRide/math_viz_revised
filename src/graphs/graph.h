/* date = July 5th 2022 2:10 pm */

#ifndef GRAPH_H
#define GRAPH_H

#include "../util/util.h"
#include "../gfx/vbo.h"
#include "../gfx/vao.h"
#include "../io/filesystem.h"
#include "linear.h"
#include "quadratic.h"
#include "bezier.h"
#include "cubic.h"

#define MAX_GRAPH_COUNT 512
#define MAX_LINES_PER_GRAPH 50
#define MAX_LINE_COUNT MAX_GRAPH_COUNT * MAX_LINES_PER_GRAPH
#define MAX_VERTEX_COUNT MAX_LINE_COUNT * 2 * 2 
#define MAX_INDEX_COUNT MAX_LINE_COUNT * 2

#define INSERT(_buffer, _index, _data) (_buffer)[(_index)] = (_data);
#define REMOVE_ARR(_arr, _index, _length) for (u32 i = (_index); i < (_length) - 1; ++i) (_arr)[i] = (_arr)[i + 1] 

enum GraphState {
    GraphState_linear,
    GraphState_quadratic,
    GraphState_bezier,
    GraphState_cubic,  
    GraphState_MAX,
};

struct Graph {
    enum GraphState state;
    
    f32 vertices[MAX_VERTEX_COUNT];
    u32 vertex_count;
    
    u32 indices[MAX_INDEX_COUNT];
    u32 index_count;
    
    b32 mesh_change_this_frame;
    
    struct Bezier *bezier;
    struct Linear *linear;
    struct Quadratic *quadratic;
    struct Cubic *cubic;

    struct VAO vao;
    struct VBO vbo, ibo;
};

void graph_init(struct Graph *self);
void graph_destroy(struct Graph *self); 
void graph_resized(struct Graph *sefl);
void graph_update(struct Graph *self);
void graph_mesh_prepare(struct Graph *self);
void graph_render(struct Graph *self);

#endif //GRAPH_H
