#ifndef _STATUS_H_
#define _STATUS_H_

#define MIN_FPS        (1.0)
#define MAX_FPS        (60.0)

struct status {
    double d_fps;
    double m_fps;
    int max_x;
    int max_y;
    void (*onkey)(struct status *status, int key);
};

void status_init();
void status_deinit();

#endif /* _STATUS_H_ */
