#include <sys/time.h>
#include <curses.h>
#include <string.h>

#include "display.h"
#include "status.h"
#include "tv.h"

static void status_onkey(struct status *status, int key)
{
    int reset;

    reset = 0;
    switch (key) {
        case '-': /* slower */
            status->d_fps = status->d_fps * 0.8;
            if (status->d_fps <  MIN_FPS)
                status->d_fps = MIN_FPS;
            reset = 1;
            break;

        case '+': /* faster */
            status->d_fps = status->d_fps * 1.2;
            if (status->d_fps >  MAX_FPS)
                status->d_fps = MAX_FPS;
            reset = 1;
            break;

        case 'q': /* quit */
            status->quit = 1;
            break;

        default:
            break;
    }
    if (reset) {
        status->frames = 0;
        memcpy(&status->then, &status->now, sizeof(status->then));
        status->m_fps = 0.0;
    }
}

static void status_display(const struct status *status, struct display *display)
{
    mvwprintw(display->win,display->max_y-1, 0, "fps:%6.3f/%6.3f",status->m_fps,status->d_fps);
}

static void status_update(struct status *status)
{
    struct timeval to;

    gettimeofday(&status->now, 0);
    status->frames++;
    tvdiff(&to, &status->now, &status->then);
    if (to.tv_sec >= 1) { /* at least 1 second has passed */
        status->m_fps = status->frames / ((double)to.tv_sec + (double)to.tv_usec / 1000000.0);
    }
}

void status_init(struct status *status)
{
    status->d_fps = MAX_FPS;
    status->m_fps = 0.0;
    status->onkey = status_onkey;
    status->display = status_display;
    status->update = status_update;
    status->frames = 0;
    gettimeofday(&status->now, 0);
    memcpy(&status->then, &status->now, sizeof(status->then));

    status->quit = 0;
}

void status_deinit()
{
    /* empty */
}
