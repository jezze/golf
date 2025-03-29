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
static unsigned int moveup = BUTTONSTATE_NONE;
static unsigned int movedown = BUTTONSTATE_NONE;
static unsigned int moveleft = BUTTONSTATE_NONE;
static unsigned int moveright = BUTTONSTATE_NONE;
static unsigned int rotleft = BUTTONSTATE_NONE;
static unsigned int rotright = BUTTONSTATE_NONE;

unsigned int game_isrunning(void)
{

    return running;

}

void game_moveup(unsigned int press)
{

    if (press && movedown == BUTTONSTATE_NONE)
        moveup = BUTTONSTATE_ACTIVE;
    else if (press && movedown == BUTTONSTATE_ACTIVE)
        moveup = BUTTONSTATE_WAIT;
    else
        moveup = BUTTONSTATE_NONE;

}

void game_movedown(unsigned int press)
{

    if (press && moveup == BUTTONSTATE_NONE)
        movedown = BUTTONSTATE_ACTIVE;
    else if (press && moveup == BUTTONSTATE_ACTIVE)
        movedown = BUTTONSTATE_WAIT;
    else
        movedown = BUTTONSTATE_NONE;

}

void game_moveleft(unsigned int press)
{

    if (press && moveright == BUTTONSTATE_NONE)
        moveleft = BUTTONSTATE_ACTIVE;
    else if (press && moveright == BUTTONSTATE_ACTIVE)
        moveleft = BUTTONSTATE_WAIT;
    else
        moveleft = BUTTONSTATE_NONE;

}

void game_moveright(unsigned int press)
{

    if (press && moveleft == BUTTONSTATE_NONE)
        moveright = BUTTONSTATE_ACTIVE;
    else if (press && moveleft == BUTTONSTATE_ACTIVE)
        moveright = BUTTONSTATE_WAIT;
    else
        moveright = BUTTONSTATE_NONE;

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

    float vx = 0;
    float vy = 0;
    float vangle = 0;

    gfx_input();

    if (moveup == BUTTONSTATE_NONE && movedown == BUTTONSTATE_WAIT)
        movedown = BUTTONSTATE_ACTIVE;

    if (movedown == BUTTONSTATE_NONE && moveup == BUTTONSTATE_WAIT)
        moveup = BUTTONSTATE_ACTIVE;

    if (moveleft == BUTTONSTATE_NONE && moveright == BUTTONSTATE_WAIT)
        moveright = BUTTONSTATE_ACTIVE;

    if (moveright == BUTTONSTATE_NONE && moveleft == BUTTONSTATE_WAIT)
        moveleft = BUTTONSTATE_ACTIVE;

    if (rotleft == BUTTONSTATE_NONE && rotright == BUTTONSTATE_WAIT)
        rotright = BUTTONSTATE_ACTIVE;

    if (rotright == BUTTONSTATE_NONE && rotleft == BUTTONSTATE_WAIT)
        rotleft = BUTTONSTATE_ACTIVE;

    if (moveup == BUTTONSTATE_ACTIVE)
        vy -= SPEED;

    if (movedown == BUTTONSTATE_ACTIVE)
        vy += SPEED;

    if (moveleft == BUTTONSTATE_ACTIVE)
        vx -= SPEED;

    if (moveright == BUTTONSTATE_ACTIVE)
        vx += SPEED;

    if (rotleft == BUTTONSTATE_ACTIVE)
        vangle += ROTSPEED;

    if (rotright == BUTTONSTATE_ACTIVE)
        vangle -= ROTSPEED;

    camera.angle += vangle;
    camera.sinphi = sin(camera.angle);
    camera.cosphi = cos(camera.angle);
    camera.x += camera.cosphi * vx + camera.sinphi * vy;
    camera.y += camera.cosphi * vy - camera.sinphi * vx;

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

