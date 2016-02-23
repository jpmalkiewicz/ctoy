#include <ncurses.h>

#include "display.h"

static void display_display(struct display *display)
{
    wrefresh(display->win);
}

void display_init(struct display *display)
{
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    display->win = stdscr;
    getmaxyx(display->win, display->max_y, display->max_x);
    display->display = display_display;
}

void display_deinit(struct display *display)
{
    endwin(); // Restore normal terminal behavior
}
