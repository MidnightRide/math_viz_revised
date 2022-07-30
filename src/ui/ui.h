/* date = July 6th 2022 2:01 pm */

#ifndef UI_H
#define UI_H

#include "../util/util.h"
#include "../graphs/graph.h"
#include "../gfx/gfx.h"

struct UI
{
    struct Graph *graph;
};

void ui_init(struct UI *self, struct Graph *graph);
void ui_destroy(struct UI *self);
void ui_update(struct UI *self);
void ui_render(struct UI *self);

#endif //UI_H
