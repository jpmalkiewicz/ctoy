#include <math.h>
#include <ncurses.h>

#include "lengthof.h"
#include "toy.h"

void
toy_init(struct toy *toy)
{
    int i;
    const char tiles[] = "0--. ";

    toy->x_velocity = 1.0;
    toy->y_velocity = 0;
    toy->x_dir = 1;
    toy->y_dir = 1;

    toy->x_pos = toy->y_pos = 0.0;
    for (i=0; i<lengthof(toy->obj); i++) {
        toy->obj[i].tile = tiles[i%lengthof(tiles)];
        toy->obj[i].pos.x = i;
        toy->obj[i].pos.y = 0;
    }
}

void
toy_deinit(struct toy *toy)
{
    return; /* nothing */
}

void toy_display(WINDOW *win, const struct toy *toy, const struct status *status)
{
    int i;

    //wclear(win); /* this is expensive! */
    for (i=1; i<lengthof(toy->obj); i++) {
        if ((toy->obj[i].pos.y != toy->obj[i-1].pos.y) || (toy->obj[i].pos.x != toy->obj[i-1].pos.x)) {
            mvwaddch(win, toy->obj[i].pos.y, toy->obj[i].pos.x, toy->obj[i].tile);
        }
    }
    mvwaddch(win, toy->obj[0].pos.y, toy->obj[0].pos.x, toy->obj[0].tile);

    mvwprintw(win,status->max_y-2,  0, "px:%6.3f vx:%6.3f",toy->x_pos,toy->x_velocity);
    mvwprintw(win,status->max_y-2, status->max_x/2, "py:%6.3f vy:%6.3f",toy->y_pos,toy->y_velocity);
    mvwprintw(win,status->max_y-1, 0, "fps:%6.3f/%6.3f",status->m_fps,status->d_fps);
    wrefresh(win);
}

void toy_update(struct toy *toy, const struct status *status)
{
    int i;

    /* shuffle */
    for (i=1; i<lengthof(toy->obj); i++) {
        toy->obj[lengthof(toy->obj)-i].pos.x = toy->obj[lengthof(toy->obj)-i-1].pos.x;
        toy->obj[lengthof(toy->obj)-i].pos.y = toy->obj[lengthof(toy->obj)-i-1].pos.y;
    }

    toy->x_pos += (toy->x_dir * toy->x_velocity);
    if ((toy->x_pos < 0.0) || ((toy->x_pos+0.5) >= (double)status->max_x)) {
        toy->x_dir = -toy->x_dir; /* reverse direction */
        if (toy->x_pos < 0.0) {
            toy->x_pos = -toy->x_pos;
        } else {
            toy->x_pos = status->max_x - (toy->x_pos - status->max_x);
        }
    }
    toy->obj[0].pos.x = (int)floor(toy->x_pos+0.5);

    toy->y_pos += (toy->y_dir * toy->y_velocity);
    if ((toy->y_pos < 0.0) || ((toy->y_pos+0.5) >= (double)status->max_y)) {
        toy->y_dir = -toy->y_dir; /* reverse direction */
        if (toy->y_pos > 0.0) {
            toy->y_pos = status->max_y - (toy->y_pos - status->max_y);
        } else {
            toy->y_pos = -toy->y_pos;
        }
    }
    toy->obj[0].pos.y = (int)floor(toy->y_pos+0.5);
}
