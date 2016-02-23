#ifndef _DISPLAY_H_
#define _DISPLAY_H_

struct display {
    WINDOW *win;
    int max_x;
    int max_y;
    void (*display)(struct display*);
};

void display_init(struct display *display);
void display_deinit(struct display *display);

#endif /* _DISPLAY_H_ */
