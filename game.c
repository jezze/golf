#include <stdlib.h>
#include <math.h>
#include "camera.h"
#include "map.h"
#include "gfx.h"

#define SPEED (32.0)
#define ROTSPEED (3.14159 / 24)

static struct camera camera;
static struct map map;
static unsigned int running = 1;

unsigned int game_isrunning(void)
{

    return running;

}

void game_moveup(unsigned int press)
{

    camera.vx = (press) ? sin(camera.angle) * -SPEED : 0;
    camera.vy = (press) ? cos(camera.angle) * -SPEED : 0;

}

void game_movedown(unsigned int press)
{

    camera.vx = (press) ? sin(camera.angle) * SPEED : 0;
    camera.vy = (press) ? cos(camera.angle) * SPEED : 0;

}

void game_moveleft(unsigned int press)
{

    camera.vx = (press) ? cos(camera.angle) * -SPEED : 0;
    camera.vy = (press) ? sin(camera.angle) * SPEED : 0;

}

void game_moveright(unsigned int press)
{

    camera.vx = (press) ? cos(camera.angle) * SPEED : 0;
    camera.vy = (press) ? sin(camera.angle) * -SPEED : 0;

}

void game_translateheight(float height)
{

    camera.z += height;

}

void game_rotateleft(unsigned int press)
{

    camera.vangle = (press) ? ROTSPEED : 0;

}

void game_rotateright(unsigned int press)
{

    camera.vangle = (press) ? -ROTSPEED : 0;

}

void game_step(void)
{

    gfx_input();

    camera.angle += camera.vangle;
    camera.x += camera.vx;
    camera.y += camera.vy;

    gfx_render(&camera, &map);

}

void game_stop(void)
{

    running = 0;

}

void game_init(void)
{

    camera_init(&camera, 2300.0, 4150.0, 200.0, 800.0);
    map_init(&map, "assets/layout.png", "assets/depth.png");
    gfx_init(SCREEN_WIDTH, SCREEN_HEIGHT);
    gfx_loadmap(&map);

}

void game_destroy(void)
{

    gfx_destroy();

}

