#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include "camera.h"
#include "map.h"
#include "ball.h"
#include "game.h"
#include "gfx.h"

#define GRASS_SIZE 4096

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Surface *layoutimg;
static SDL_Surface *depthimg;
static SDL_Texture *field;
static SDL_Texture *minimap;
static SDL_Rect fieldrect;
static SDL_Rect minimaprect;
static int zbuffer[SCREEN_WIDTH];
static unsigned int horizon;
static unsigned char grass[GRASS_SIZE];
static unsigned int xorstate = 0x01234567;

static unsigned char xorshift(void)
{

    unsigned int x = xorstate;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return xorstate = x;

}

static unsigned int gettype(struct map *map, float cx, float cy)
{

    unsigned int mx = cx;
    unsigned int my = cy;

    if (mx < map->w && my < map->h)
    {

        unsigned int mapoffset = my * map->w + mx;

        return map->layoutpixels[mapoffset];

    }

    return MAP_TYPE_NONE;

}

static float getgrassheight(unsigned int type, float cx, float cy)
{

    unsigned int mx = cx * cx;
    unsigned int my = cy * cy;
    unsigned int offset = (my * 32 + mx) & 0xFFF;
    unsigned char height = grass[offset];

    switch (type)
    {

    case MAP_TYPE_FAIRWAY:
        return height & 0x03;

    case MAP_TYPE_GREEN:
        return height & 0x01;

    case MAP_TYPE_ROUGH:
        return height & 0x07;

    case MAP_TYPE_DEEPROUGH:
        return height & 0x0f;

    case MAP_TYPE_SAND:
        return height & 0x01;

    }

    return 0;

}

static unsigned int getcolor(unsigned int type, float cx, float cy, float grassheight)
{

    unsigned int mx = cx;
    unsigned int my = cy;
    unsigned char r = 0x40;
    unsigned char g = 0x80;
    unsigned char b = 0xA0;
    unsigned char a = 0xFF;

    switch (type)
    {

    case MAP_TYPE_NONE:
        return 0x4080A0FF;

    case MAP_TYPE_FAIRWAY:
        r = 0x00;
        g = 0x60;
        b = 0x00;

        g -= grassheight * 4;

        if ((mx / 16) % 2 == 0)
            g -= 0x02;

        if ((my / 16) % 2 == 0)
            g -= 0x02;

        break;

    case MAP_TYPE_GREEN:
        r = 0x00;
        g = 0x62;
        b = 0x00;

        g -= grassheight * 2;

        break;

    case MAP_TYPE_ROUGH:
        r = 0x00;
        g = 0x50;
        b = 0x00;

        g -= grassheight * 2;

        break;

    case MAP_TYPE_DEEPROUGH:
        r = 0x00;
        g = 0x40;
        b = 0x00;

        g -= grassheight * 2;

        break;

    case MAP_TYPE_SAND:
        r = 0xC0;
        g = 0xC0;
        b = 0x80;

        r -= grassheight * 8;
        g -= grassheight * 8;
        b -= grassheight * 8;

        break;

    case MAP_TYPE_WATER:
        return 0x102030FF;

    case MAP_TYPE_HOLE:
        return 0xE0E0E0FF;

    }

    return ((r << 24) | (g << 16) | (b << 8) | a);

}

static void paintsky(unsigned int *pixels)
{

    unsigned int i;

    for (i = 0; i < fieldrect.w * (horizon + 1); i++)
        pixels[i] = 0x4080A0FF;

}

static void paintline(unsigned int *pixels, unsigned int offset, unsigned int pitch, unsigned int color, unsigned int count)
{

    int i;

    for (i = 0; i < count; i++)
    {

        pixels[offset] = color;

        offset += pitch;

    }

}

void renderfield(struct camera *camera, struct map *map)
{

    unsigned int x;
    unsigned int y;
    unsigned int *pixels;
    int pitch;
    float plx;
    float ply;
    float prx;
    float pry;
    float dx;
    float dy;
    float cx;
    float cy;
    SDL_Rect targetrect;

    targetrect.x = 0;
    targetrect.y = 0;
    targetrect.w = fieldrect.w;
    targetrect.h = fieldrect.h;

    for (x = 0; x < fieldrect.w; x++)
        zbuffer[x] = fieldrect.h;

    SDL_LockTexture(field, &fieldrect, (void **)&pixels, &pitch);
    paintsky(pixels);

    for (y = fieldrect.h + 128; y > horizon; y--)
    {

        float z = (camera->distance * camera->z) / (y - horizon);

        plx = (camera->cosphi * -z) - (camera->sinphi * z);
        ply = (camera->sinphi * z) - (camera->cosphi * z);
        prx = (camera->cosphi * z) - (camera->sinphi * z);
        pry = (camera->sinphi * -z) - (camera->cosphi * z);
        dx = (prx - plx) / fieldrect.w;
        dy = (pry - ply) / fieldrect.w;
        cx = plx + camera->x;
        cy = ply + camera->y;

        for (x = 0; x < fieldrect.w; x++)
        {

            unsigned int type = gettype(map, cx, cy);
            float mapheight = map_getheight(map, cx, cy);
            float grassheight = getgrassheight(type, cx, cy);
            float height = (mapheight + grassheight) * ((float)y / z) * 10;
            unsigned int ztop = (float)y - height;

            if (ztop < zbuffer[x])
            {

                unsigned int color = getcolor(type, cx, cy, grassheight);
                unsigned int offset = (ztop * fieldrect.w) + x;

                paintline(pixels, offset, fieldrect.w, color, zbuffer[x] - ztop);

                zbuffer[x] = ztop;

            }

            cx += dx;
            cy += dy;

        }

    }

    SDL_UnlockTexture(field);
    SDL_RenderCopy(renderer, field, &fieldrect, &targetrect);

}

void renderminimap(struct camera *camera, struct map *map)
{

    unsigned int x;
    unsigned int y;
    unsigned int *pixels;
    int pitch;
    SDL_Rect targetrect;
    unsigned int scale = 20;
    float cx = 0;
    float cy = 0;
    float dx = scale;
    float dy = scale;

    targetrect.x = SCREEN_WIDTH - minimaprect.w - SCREEN_PADDING;
    targetrect.y = SCREEN_HEIGHT - minimaprect.h - SCREEN_PADDING;
    targetrect.w = minimaprect.w;
    targetrect.h = minimaprect.h;

    SDL_LockTexture(minimap, &minimaprect, (void **)&pixels, &pitch);

    for (y = 0; y < minimaprect.h; y++)
    {

        for (x = 0; x < minimaprect.w; x++)
        {

            unsigned int type = gettype(map, cx, cy);
            float grassheight = getgrassheight(type, cx, cy);
            unsigned int offset = (y * minimaprect.w) + x;

            pixels[offset] = getcolor(type, cx, cy, grassheight);

            cx += dx;

        }

        cx = 0;
        cy += dy;

    }

    SDL_UnlockTexture(minimap);
    SDL_RenderCopy(renderer, minimap, &minimaprect, &targetrect);

}

static unsigned int frametime = 1000 / 60;

void gfx_render(struct camera *camera, struct map *map, struct ball *ball)
{

    unsigned int framestart = SDL_GetTicks();
    unsigned int frameend;
    unsigned int frametotal;

    SDL_SetRenderDrawColor(renderer, 0x40, 0x80, 0xA0, 0xFF);
    SDL_RenderClear(renderer);
    renderfield(camera, map);
    renderminimap(camera, map);
    SDL_RenderPresent(renderer);

    frameend = SDL_GetTicks();
    frametotal = frameend - framestart;

    if (frametotal < frametime)
        SDL_Delay(frametime - frametotal);

}

void gfx_input(void)
{

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {

        switch (event.type)
        {

        case SDL_QUIT:
            game_stop();

            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {

            case SDLK_w:
                game_moveup(1);

                break;

            case SDLK_s:
                game_movedown(1);

                break;

            case SDLK_a:
                game_moveleft(1);

                break;

            case SDLK_d:
                game_moveright(1);

                break;

            case SDLK_q:
                game_rotateleft(1);

                break;

            case SDLK_e:
                game_rotateright(1);

                break;

            case SDLK_ESCAPE:
                game_stop();

                break;

            }

            break;

        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {

            case SDLK_w:
                game_moveup(0);

                break;

            case SDLK_s:
                game_movedown(0);

                break;

            case SDLK_a:
                game_moveleft(0);

                break;

            case SDLK_d:
                game_moveright(0);

                break;

            case SDLK_u:
                game_translateheight(100);

                break;

            case SDLK_i:
                game_translateheight(-100);

                break;

            case SDLK_q:
                game_rotateleft(0);

                break;

            case SDLK_e:
                game_rotateright(0);

                break;

            }

            break;

        }

    }

}

void gfx_loadmap(struct map *map)
{

    layoutimg = IMG_Load(map->layout);

    if (!layoutimg)
    {

        printf("Layout image could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

    depthimg = IMG_Load(map->depth);

    if (!depthimg)
    {

        printf("Depth image could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

    map->layoutpixels = (unsigned char *)layoutimg->pixels;
    map->depthpixels = (unsigned char *)depthimg->pixels;

}

void gfx_init(unsigned int w, unsigned int h)
{

    unsigned int i;

    horizon = h / 4;
    fieldrect.x = 0;
    fieldrect.y = 0;
    fieldrect.w = w;
    fieldrect.h = h;
    minimaprect.x = 0;
    minimaprect.y = 0;
    minimaprect.w = w / 8;
    minimaprect.h = h / 6;

    for (i = 0; i < GRASS_SIZE; i++)
        grass[i] = xorshift();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {

        printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

    window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);

    if (!window)
    {

        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {

        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

    field = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, fieldrect.w, fieldrect.h);

    if (!field)
    {

        printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

    minimap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, minimaprect.w, minimaprect.h);

    if (!minimap)
    {

        printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

}

void gfx_destroy(void)
{

    SDL_DestroyTexture(field);
    SDL_DestroyTexture(minimap);
    SDL_FreeSurface(layoutimg);
    SDL_FreeSurface(depthimg);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

