/* date = July 5th 2022 1:33 pm */
#ifndef STATE_H
#define STATE_H

#include "gfx/gfx.h"
#include "gfx/window.h"
#include "gfx/renderer.h"
#include "util/util.h"
#include "graphs/graph.h"
#include "ui/ui.h"

struct State {
    struct Window *window;
    struct UI ui;
    struct Graph graph;
    struct Renderer renderer;
    u64 ticks;
};

extern struct State state;

#endif