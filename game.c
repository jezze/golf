#include <stdlib.h>
#include <math.h>
#include "camera.h"
#include "gfx.h"

#define SPEED (32.0)
#define ROTSPEED (3.14159 / 24)

static struct camera camera;
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

    gfx_render(&camera);

}

void game_stop(void)
{

    running = 0;

}

void game_init(void)
{

    gfx_init(SCREEN_WIDTH, SCREEN_HEIGHT);

    camera.x = 2300;
    camera.y = 4150;
    camera.z = 200;
    camera.distance = 800;

}

void game_destroy(void)
{

    gfx_destroy();

}

