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

void loop(WINDOW *win)
{
    int i;
    int x[5], y[5];
    int ch[5];
    double x_pos, y_pos;
    double x_velocity, y_velocity;
    double x_dir, y_dir;
    int max_x, max_y;
    unsigned int udelay;
    unsigned int frames;
    struct timeval now, then, to;
    double d_fps, m_fps;
    int quit;

    x_velocity = 1;
    y_velocity = 0;
    x_dir = 1;
    y_dir = 1;
    x_pos = y_pos = 0.0;
    memset(x, 0, sizeof(x));
    memset(y, 0, sizeof(y));
    ch[0] = 'O';
    ch[1] = 'o';
    ch[2] = '_';
    ch[3] = '.';
    ch[4] = ' ';
    d_fps = MAX_FPS;
    m_fps = 0.0;
    getmaxyx(win, max_y, max_x);
    quit = 0;
    frames = 0;
    gettimeofday(&then, 0);
    while(!quit) {
        //wclear(win); /* this is expensive! */
        for (i=1; i<lengthof(x); i++) {
            if ((y[i] != y[i-1]) || (x[i] != x[i-1])) {
                mvwaddch(win, y[i], x[i], ch[i]);
            }
        }
        mvwaddch(win, y[0], x[0], ch[0]);
        /* shuffle */
        for (i=1; i<lengthof(x); i++) {
            x[lengthof(x)-i] = x[lengthof(x)-i-1];
            y[lengthof(x)-i] = y[lengthof(x)-i-1];
        }

        mvwprintw(win,max_y-2,  0, "px:%6.3f vx:%6.3f",x_pos,x_velocity);
        mvwprintw(win,max_y-2, max_x/2, "py:%6.3f vy:%6.3f",y_pos,y_velocity);
        mvwprintw(win,max_y-1, 0, "fps:%6.3f/%6.3f",m_fps,d_fps);
        wrefresh(win);
        gettimeofday(&now, 0);
        frames++;
        tvdiff(&to, &now, &then);
        if (to.tv_sec >= 1) { /* at least 1 second has passed */
            m_fps = frames / ((double)to.tv_sec + (double)to.tv_usec / 1000000.0);
        }

        udelay = 1000000 / (unsigned int)floor(d_fps+0.5);
        switch (get_key(STDIN_FILENO, udelay)) {
            case 'q': /* quit */
                quit = 1;
                break;
            case 'h': /* left */
                x_velocity = x_velocity * 0.8;
                if (x_velocity < MIN_X_VELOCITY)
                    x_velocity = MIN_X_VELOCITY;
                break;
            case 'j': /* down */
                if (y_velocity > 0.0)
                    y_velocity = y_velocity * 1.2;
                else
                    y_velocity = 0.2;
                if (y_velocity > MAX_Y_VELOCITY)
                    y_velocity = MAX_Y_VELOCITY;
                break;
            case 'k': /* up */
                y_velocity = y_velocity * 0.8;
                if (y_velocity < MIN_Y_VELOCITY)
                    y_velocity = 0.0;
                break;
            case 'l': /* right */
                x_velocity = x_velocity * 1.2;
                if (x_velocity > MAX_X_VELOCITY)
                    x_velocity = MAX_X_VELOCITY;
                break;
            case 's': /* slower */
                d_fps = d_fps * 0.8;
                if (d_fps <  MIN_FPS)
                    d_fps = MIN_FPS;
                frames = 0;
                /* restart frame counter */
                m_fps = 0.0;
                frames = 0;
                memcpy(&then, &now, sizeof(then));
                break;
            case 'S': /* faster */
                d_fps = d_fps * 1.2;
                if (d_fps >  MAX_FPS)
                    d_fps = MAX_FPS;
                /* restart frame counter */
                m_fps = 0.0;
                frames = 0;
                memcpy(&then, &now, sizeof(then));
                break;
            default:
                break;
        }
        x_pos += (x_dir * x_velocity);
        if ((x_pos < 0.0) || ((x_pos+0.5) >= (double)max_x)) {
            x_dir = -x_dir; /* reverse direction */
            if (x_pos < 0.0) {
                x_pos = -x_pos;
            } else {
                x_pos = max_x - (x_pos - max_x);
            }
        }
        x[0] = (int)floor(x_pos+0.5);

        y_pos += (y_dir * y_velocity);
        if ((y_pos < 0.0) || ((y_pos+0.5) >= (double)max_y)) {
            y_dir = -y_dir; /* reverse direction */
            if (y_pos > 0.0) {
                y_pos = max_y - (y_pos - max_y);
            } else {
                y_pos = -y_pos;
            }
        }
        y[0] = (int)floor(y_pos+0.5);
    }
}

int main(int argc, char *argv[])
{
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    loop(stdscr);

    endwin(); // Restore normal terminal behavior
    return 0;
}
