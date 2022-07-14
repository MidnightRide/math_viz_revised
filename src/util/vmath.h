/* date = July 8th 2022 0:31 pm */

#ifndef VMATH_H
#define VMATH_H

#define bezier4(_p0, _p1, _p2, _p3, _t) glms_vec2_add(glms_vec2_add(glms_vec2_scale((_p0), (1-(_t))*(1-(_t))*(1-(_t))), glms_vec2_scale((_p1), 3*(1-(_t))*(1-(_t))*(_t))), glms_vec2_add(glms_vec2_scale((_p2), 3*(1-(_t))*(_t)*(_t)), glms_vec2_scale((_p3), (_t)*(_t)*(_t))))
static inline b8 glms_vec2_eqve(vec2s v0, vec2s v1, f32 e) {
    return (v0.x + e > v1.x &&
            v0.x - e < v1.x &&
            v0.y + e > v1.y &&
            v0.y - e < v1.y);
}

#endif //VMATH_H
