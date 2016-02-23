#ifndef _STATUS_H_
#define _STATUS_H_

#define MIN_FPS        (1.0)
#define MAX_FPS        (60.0)

struct display;
struct timeval;

struct status {
    double d_fps;
    double m_fps;

    unsigned int frames;
    struct timeval now;
    struct timeval then;

    int quit;

    void (*onkey)(struct status *status, int key);
    void (*display)(const struct status *status, struct display *display);
    void (*update)(struct status *status);
};

void status_init();
void status_deinit();

#endif /* _STATUS_H_ */
