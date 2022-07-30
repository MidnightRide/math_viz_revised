/* date = June 30th 2022 1:44 pm */

#ifndef WINDOW_H
#define WINDOW_H

#include "./gfx.h"
#include "../util/util.h"

struct Button {
    b8 down, last, last_tick, pressed, pressed_tick;
};

struct Mouse {
    struct Button buttons[GLFW_MOUSE_BUTTON_LAST];
    vec2s position, delta;
};

struct Keyboard {
    struct Button keys[GLFW_KEY_LAST];
};

typedef void (*FWindow)();

struct Window {
    GLFWwindow *handle;
    ivec2s size;
    FWindow init, destroy, tick, update, render, resized;
    struct Mouse mouse;
    struct Keyboard keyboard;
    
    u64 last_second;
    u64 frames, fps, last_frame, frame_delta;
    u64 ticks, tps, tick_remainder;
};

// global window
extern struct Window window;

void window_create(FWindow init, FWindow destroy, FWindow tick, FWindow update, FWindow render, FWindow resized);
void window_loop();
void mouse_set_grabbed(b8 grabbed);
bool mouse_get_grabbed();

#endif //WINDOW_H
