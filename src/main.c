#include "gfx/gfx.h"
#include "gfx/window.h"

#include "state.h"

struct State state;

void init() {
    state.window = &window;
    renderer_init(&state.renderer);
    graph_init(&state.graph);
    ui_init(&state.ui, &state.graph);

    state.renderer.clear_color = (vec4s){{0.15f, 0.15f, 0.15f, 1.f}};
}

void destroy() {
    renderer_destroy(&state.renderer);
    graph_destroy(&state.graph);
    ui_destroy(&state.ui);
}

void resized() { graph_resized(&state.graph); }

void tick() { state.ticks++; }

void update() {
    renderer_update(&state.renderer);
    graph_update(&state.graph);
    ui_update(&state.ui);
}

void render() {
    renderer_prepare(&state.renderer, RenderPass_3d);
    renderer_push_camera(&state.renderer);
    {
        renderer_set_camera(&state.renderer, CameraType_perspective);
        renderer_set_view_proj(&state.renderer);
    }
    renderer_pop_camera(&state.renderer);
    renderer_prepare(&state.renderer, RenderPass_2d);
    renderer_push_camera(&state.renderer);
    {
        renderer_set_camera(&state.renderer, CameraType_ortho);
        graph_render(&state.graph);
        ui_render(&state.ui);
    }
    renderer_pop_camera(&state.renderer);
}

int main(int argc, char *argv[]) {
    window_create(init, destroy, tick, update, render, resized);
    window_loop();
}
