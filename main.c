#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>

#include "lengthof.h"
#include "display.h"
#include "status.h"
#include "toy.h"
#include "tv.h"

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

void loop(struct display *display, struct status *status, struct toy *toy)
{
    int key;
    unsigned int udelay;

    toy->display(toy, display);
    status->display(status, display);
    display->display(display);

    toy->update(toy, display);
    status->update(status);

    udelay = 1000000 / (unsigned int)floor(status->d_fps+0.5);
    key = get_key(STDIN_FILENO, udelay);

    toy->onkey(toy, key);
    status->onkey(status, key);
}

int main(int argc, char *argv[])
{
    struct display display;
    struct status status;
    struct toy toy;

    display_init(&display);
    toy_init(&toy);
    status_init(&status);

    while (!status.quit) {
        loop(&display, &status, &toy);
    }

    status_deinit(&status);
    toy_deinit(&toy);
    display_deinit(&display);
    return 0;
}
