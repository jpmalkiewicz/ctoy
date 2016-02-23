#ifndef _TOY_H_
#define _TOY_H_

#define MIN_X_VELOCITY (0.2)
#define MAX_X_VELOCITY (10.0)
#define MIN_Y_VELOCITY (0.2)
#define MAX_Y_VELOCITY (10.0)

struct status;

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
    void (*onkey)(struct toy*, int key);
    void (*update)(struct toy*, const struct status *status);
    void (*display)(const struct toy*, const struct status *status, WINDOW *win);
};


void toy_init(struct toy *toy);
void toy_deinit(struct toy *toy);

#endif /* _TOY_H_ */
