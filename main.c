#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>

#include "lengthof.h"
#include "toy.h"

void tvdiff(struct timeval *td, const struct timeval *t1, const struct timeval *t2)
{
    if (t1->tv_usec > t2->tv_usec) {
        td->tv_usec = t1->tv_usec - t2->tv_usec;
        td->tv_sec = t1->tv_sec - t2->tv_sec;
    } else {
        td->tv_usec = 1000000 + t1->tv_usec - t2->tv_usec;
        td->tv_sec = t1->tv_sec - t2->tv_sec - 1;
    }
}

int get_key(int fd, unsigned int udelay)
{
    int k;
    fd_set rfd;
    struct timeval to, now, then;

    k = 0;
    to.tv_sec = 0;
    to.tv_usec = udelay;

    FD_ZERO(&rfd);
    FD_SET(fd, &rfd);
    gettimeofday(&then, NULL);
    if (select(fd+1, &rfd, 0, 0, &to) > 0) {
        k = getch();
    }
    gettimeofday(&now, NULL);
    tvdiff(&to, &now, &then);
    if (to.tv_sec) /* something is wrong! */
        sleep(to.tv_sec);
    if (to.tv_usec < udelay) /* sleep the remainder off */
        usleep(udelay - to.tv_usec);

    return k;
}

void loop(WINDOW *win, struct toy *toy)
{
    unsigned int udelay;
    unsigned int frames;
    struct timeval now, then, to;
    struct status status;
    int quit;

    status.d_fps = MAX_FPS;
    status.m_fps = 0.0;
    getmaxyx(win, status.max_y, status.max_x);
    quit = 0;
    frames = 0;
    gettimeofday(&then, 0);
    while(!quit) {
        toy_display(win, toy, &status);
        toy_update(toy, &status);

        gettimeofday(&now, 0);
        frames++;
        tvdiff(&to, &now, &then);
        if (to.tv_sec >= 1) { /* at least 1 second has passed */
            status.m_fps = frames / ((double)to.tv_sec + (double)to.tv_usec / 1000000.0);
        }

        udelay = 1000000 / (unsigned int)floor(status.d_fps+0.5);
        switch (get_key(STDIN_FILENO, udelay)) {
            case 'q': /* quit */
                quit = 1;
                break;
            case 'h': /* left */
                toy->x_velocity = toy->x_velocity * 0.8;
                if (toy->x_velocity < MIN_X_VELOCITY)
                    toy->x_velocity = MIN_X_VELOCITY;
                break;
            case 'j': /* down */
                if (toy->y_velocity > 0.0)
                    toy->y_velocity = toy->y_velocity * 1.2;
                else
                    toy->y_velocity = 0.2;
                if (toy->y_velocity > MAX_Y_VELOCITY)
                    toy->y_velocity = MAX_Y_VELOCITY;
                break;
            case 'k': /* up */
                toy->y_velocity = toy->y_velocity * 0.8;
                if (toy->y_velocity < MIN_Y_VELOCITY)
                    toy->y_velocity = 0.0;
                break;
            case 'l': /* right */
                toy->x_velocity = toy->x_velocity * 1.2;
                if (toy->x_velocity > MAX_X_VELOCITY)
                    toy->x_velocity = MAX_X_VELOCITY;
                break;
            case 's': /* slower */
                status.d_fps = status.d_fps * 0.8;
                if (status.d_fps <  MIN_FPS)
                    status.d_fps = MIN_FPS;
                frames = 0;
                /* restart frame counter */
                status.m_fps = 0.0;
                frames = 0;
                memcpy(&then, &now, sizeof(then));
                break;
            case 'S': /* faster */
                status.d_fps = status.d_fps * 1.2;
                if (status.d_fps >  MAX_FPS)
                    status.d_fps = MAX_FPS;
                /* restart frame counter */
                status.m_fps = 0.0;
                frames = 0;
                memcpy(&then, &now, sizeof(then));
                break;
            default:
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    struct toy toy;

    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    toy_init(&toy);

    loop(stdscr, &toy);

    toy_deinit(&toy);

    endwin(); // Restore normal terminal behavior
    return 0;
}
