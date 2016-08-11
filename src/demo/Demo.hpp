#ifndef DEMO_H
#define DEMO_H

#define WIDTH 1024
#define HEIGHT 768
#define NAME "MAIN"
#define BG_COLOR vec4(0.0, 0.0, 0.4, 0.0)

void particle_demo();
void rotation_demo();
void billboard_demo();
void shadow_map_demo();
void pick_by_color_demo();
void pick_by_bullet_demo();
void pick_by_udf_demo();
void standard_demo();

#endif