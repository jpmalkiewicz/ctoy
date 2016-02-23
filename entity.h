#ifndef _ENTITY_H_
#define _ENTITY_H_

struct display;
struct entity;

struct entity_methods {
    void (*onkey)(struct entity*, int key);
    void (*update)(struct entity*, const struct display *display);
    void (*display)(const struct entity*, struct display *display);
    struct entity *(*deinit)(struct entity*);
};

struct entity {
    const struct entity_methods *m;
};

#endif /*  _ENTITY_H_ */
