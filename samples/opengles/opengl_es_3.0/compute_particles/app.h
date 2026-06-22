
#ifndef APP_H
#define APP_H

bool load_app();
void init_app(int width, int height);
void update_app(float delta_time, float total_time);
void render_app(float delta_time, float total_time);
void free_app();
void on_pointer_down(float x, float y);
void on_pointer_up(float x, float y);

#endif
