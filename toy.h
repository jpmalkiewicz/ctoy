#ifndef _TOY_H_
#define _TOY_H_

#define MIN_FPS        (1.0)
#define MAX_FPS        (60.0)
#define MIN_X_VELOCITY (0.2)
#define MAX_X_VELOCITY (10.0)
#define MIN_Y_VELOCITY (0.2)
#define MAX_Y_VELOCITY (10.0)

struct obj {
    int tile;
    struct {
        int x;
        int y;
    } pos;
};

struct toy {
    struct obj obj[5];
    double x_pos, y_pos;
    double x_velocity, y_velocity;
    double x_dir, y_dir;
};

struct status {
    double d_fps;
    double m_fps;
    int max_x;
    int max_y;
};

void toy_init(struct toy *toy);
void toy_deinit(struct toy *toy);
void toy_display(WINDOW *win, const struct toy *toy, const struct status *status);
void toy_update(struct toy *toy, const struct status *status);

#endif /* _TOY_H_ */
