#include <stdlib.h>
#include <math.h>
#include "camera.h"
#include "map.h"
#include "gfx.h"

#define SPEED (32.0)
#define ROTSPEED (3.14159 / 24)

#define BUTTONSTATE_NONE 0
#define BUTTONSTATE_ACTIVE 1
#define BUTTONSTATE_WAIT 2

static struct camera camera;
static struct map map;
static unsigned int running = 1;
static unsigned int rotleft = BUTTONSTATE_NONE;
static unsigned int rotright = BUTTONSTATE_NONE;
static float vx;
static float vy;

unsigned int game_isrunning(void)
{

    return running;

}

void game_moveup(unsigned int press)
{

    vy = (press) ? -SPEED : 0;

}

void game_movedown(unsigned int press)
{

    vy = (press) ? SPEED : 0;

}

void game_moveleft(unsigned int press)
{

    vx = (press) ? -SPEED : 0;

}

void game_moveright(unsigned int press)
{

    vx = (press) ? SPEED : 0;

}

void game_translateheight(float height)
{

    camera.z += height;

}

void game_rotateleft(unsigned int press)
{

    if (press && rotright == BUTTONSTATE_NONE)
        rotleft = BUTTONSTATE_ACTIVE;
    else if (press && rotright == BUTTONSTATE_ACTIVE)
        rotleft = BUTTONSTATE_WAIT;
    else
        rotleft = BUTTONSTATE_NONE;

}

void game_rotateright(unsigned int press)
{

    if (press && rotleft == BUTTONSTATE_NONE)
        rotright = BUTTONSTATE_ACTIVE;
    else if (press && rotleft == BUTTONSTATE_ACTIVE)
        rotright = BUTTONSTATE_WAIT;
    else
        rotright = BUTTONSTATE_NONE;

}

void game_step(void)
{

    gfx_input();

    if (rotleft == BUTTONSTATE_NONE && rotright == BUTTONSTATE_WAIT)
        rotright = BUTTONSTATE_ACTIVE;

    if (rotright == BUTTONSTATE_NONE && rotleft == BUTTONSTATE_WAIT)
        rotleft = BUTTONSTATE_ACTIVE;

    camera.vangle = 0;

    if (rotleft == BUTTONSTATE_ACTIVE)
        camera.vangle += ROTSPEED;

    if (rotright == BUTTONSTATE_ACTIVE)
        camera.vangle -= ROTSPEED;

    camera.angle += camera.vangle;
    camera.x += cos(camera.angle) * vx + sin(camera.angle) * vy;
    camera.y += cos(camera.angle) * vy - sin(camera.angle) * vx;

    gfx_render(&camera, &map);

}

void game_stop(void)
{

    running = 0;

}

void game_init(void)
{

    map_init(&map, "assets/layout.png", "assets/depth.png", 2300.0, 4150.0);
    camera_init(&camera, map.teex, map.teey + 120, 200.0, 800.0);
    gfx_init(SCREEN_WIDTH, SCREEN_HEIGHT);
    gfx_loadmap(&map);

}

void game_destroy(void)
{

    gfx_destroy();

}

