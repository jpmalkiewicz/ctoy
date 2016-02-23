#ifndef _TOY_PRIV_H_
#define _TOY_PRIV_H_

#define MIN_X_VELOCITY (0.2)
#define MAX_X_VELOCITY (10.0)
#define MIN_Y_VELOCITY (0.2)
#define MAX_Y_VELOCITY (10.0)

struct status;
struct display;

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
    struct entity entity;
};

#endif /* _TOY_PRIV_H_ */
