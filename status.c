#include <sys/time.h>
#include "status.h"

static void status_onkey(struct status *status, int key)
{
    switch (key) {
        case '-': /* slower */
            status->d_fps = status->d_fps * 0.8;
            if (status->d_fps <  MIN_FPS)
                status->d_fps = MIN_FPS;
            status->m_fps = 0.0;
            break;

        case '+': /* faster */
            status->d_fps = status->d_fps * 1.2;
            if (status->d_fps >  MAX_FPS)
                status->d_fps = MAX_FPS;
            status->m_fps = 0.0;
            break;

        default:
            break;
    }
}

void status_init(struct status *status)
{
    status->d_fps = MAX_FPS;
    status->m_fps = 0.0;
    status->onkey = status_onkey;
}

void status_deinit()
{
    /* empty */
}
