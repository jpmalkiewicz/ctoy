#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>

#define MIN_FPS        (1.0)
#define MAX_FPS        (60.0)
#define MIN_X_VELOCITY (0.2)
#define MAX_X_VELOCITY (10.0)
#define MIN_Y_VELOCITY (0.2)
#define MAX_Y_VELOCITY (10.0)

#ifndef lengthof
#define lengthof(x)   (sizeof(x)/sizeof(*(x)))
#endif /* lengthof */

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
