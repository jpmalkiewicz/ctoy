#include <sys/time.h>
#include <math.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>

#include "lengthof.h"
#include "containerof.h"

#include "entity.h"
#include "display.h"

#include "toy_priv.h"
#include "toy.h"

static void toy_display(const struct entity *entity, struct display *display)
{
    int i;
    const struct toy *toy;

    toy = containerof(entity, struct toy, entity);

    for (i=1; i<lengthof(toy->obj); i++) {
        if ((toy->obj[i].pos.y != toy->obj[i-1].pos.y) || (toy->obj[i].pos.x != toy->obj[i-1].pos.x)) {
            mvwaddch(display->win, toy->obj[i].pos.y, toy->obj[i].pos.x, toy->obj[i].tile);
        }
    }
    mvwaddch(display->win, toy->obj[0].pos.y, toy->obj[0].pos.x, toy->obj[0].tile);

    mvwprintw(display->win,display->max_y-2,  0, "px:%6.3f vx:%6.3f",toy->x_pos,toy->x_velocity);
    mvwprintw(display->win,display->max_y-2, display->max_x/2, "py:%6.3f vy:%6.3f",toy->y_pos,toy->y_velocity);
}

static void toy_update(struct entity *entity, const struct display *display)
{
    int i;
    struct toy *toy;

    toy = containerof(entity, struct toy, entity);

    /* shuffle */
    for (i=1; i<lengthof(toy->obj); i++) {
        toy->obj[lengthof(toy->obj)-i].pos.x = toy->obj[lengthof(toy->obj)-i-1].pos.x;
        toy->obj[lengthof(toy->obj)-i].pos.y = toy->obj[lengthof(toy->obj)-i-1].pos.y;
    }

    toy->x_pos += (toy->x_dir * toy->x_velocity);
    if ((toy->x_pos < 0.0) || ((toy->x_pos+0.5) >= (double)display->max_x)) {
        toy->x_dir = -toy->x_dir; /* reverse direction */
        if (toy->x_pos < 0.0) {
            toy->x_pos = -toy->x_pos;
        } else {
            toy->x_pos = display->max_x - (toy->x_pos - display->max_x);
        }
    }
    toy->obj[0].pos.x = (int)floor(toy->x_pos+0.5);

    toy->y_pos += (toy->y_dir * toy->y_velocity);
    if ((toy->y_pos < 0.0) || ((toy->y_pos+0.5) >= (double)display->max_y)) {
        toy->y_dir = -toy->y_dir; /* reverse direction */
        if (toy->y_pos > 0.0) {
            toy->y_pos = display->max_y - (toy->y_pos - display->max_y);
        } else {
            toy->y_pos = -toy->y_pos;
        }
    }
    toy->obj[0].pos.y = (int)floor(toy->y_pos+0.5);
}

static void toy_onkey(struct entity *entity, int key)
{
    struct toy *toy;

    toy = containerof(entity, struct toy, entity);

    switch (key) {
        case 'h': /* left */
            toy->x_velocity = toy->x_velocity * 0.8;
            if (toy->x_velocity < MIN_X_VELOCITY)
                toy->x_velocity = MIN_X_VELOCITY;
            break;

        case 'j': /* down */
            toy->y_velocity = toy->y_velocity * 0.8;
            if (toy->y_velocity < MIN_Y_VELOCITY)
                toy->y_velocity = 0.0;
            break;

        case 'k': /* up */
            if (toy->y_velocity > 0.0)
                toy->y_velocity = toy->y_velocity * 1.2;
            else
                toy->y_velocity = 0.2;
            if (toy->y_velocity > MAX_Y_VELOCITY)
                toy->y_velocity = MAX_Y_VELOCITY;
            break;

        case 'l': /* right */
            toy->x_velocity = toy->x_velocity * 1.2;
            if (toy->x_velocity > MAX_X_VELOCITY)
                toy->x_velocity = MAX_X_VELOCITY;
            break;

        default:
            break;
    }
}

static struct entity*
toy_deinit(struct entity *entity)
{
    struct toy *toy;

    toy = containerof(entity, struct toy, entity);
    free(toy);

    return NULL;
}

static const struct entity_methods toy_methods = {
    .onkey = toy_onkey,
    .update = toy_update,
    .display = toy_display,
    .deinit = toy_deinit,
};

struct entity*
toy_init(void)
{
    int i;
    struct toy *toy;
    const char tiles[] = "0--. ";

    if ((toy=malloc(sizeof(*toy))) == NULL) {
        return NULL;
    }
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
    toy->entity.m = &toy_methods;
    return &toy->entity;
}
