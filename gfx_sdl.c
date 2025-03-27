#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include "camera.h"
#include "game.h"
#include "gfx.h"

#define TYPE_NONE 0
#define TYPE_FAIRWAY 1
#define TYPE_GREEN 2
#define TYPE_ROUGH 3
#define TYPE_DEEPROUGH 4
#define TYPE_SAND 5
#define TYPE_RED 6
#define TYPE_HOLE 7

static unsigned int colors[] = {
    0x4080A0FF,
    0x006000FF,
    0x008000FF,
    0x004000FF,
    0x002000FF,
    0x808040FF,
    0xF00000FF,
    0xE0E0E0FF
};

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Surface *layoutimg;
static SDL_Surface *depthimg;
static SDL_Texture *field;
static SDL_Rect skyrect;
static SDL_Rect fieldrect;
static SDL_Rect drect;
static unsigned char *layoutpixels;
static unsigned char *depthpixels;
static int heightbuffer[SCREEN_WIDTH];

static unsigned int getcolor(float cx, float cy)
{

    unsigned int mx = cx;
    unsigned int my = cy;

    if (mx < layoutimg->w && my < layoutimg->h)
    {

        unsigned int mapoffset = my * layoutimg->w + mx;
        unsigned char type = layoutpixels[mapoffset];

        if (type == 1)
        {

            unsigned int color = colors[type];

            if ((mx / 16) % 2 == 0)
            {

                color = color - 0x00040000;

            }

            if ((my / 16) % 2 == 0)
            {

                color = color - 0x00040000;

            }

            return color;

        }

        return colors[type];
 
    }

    return colors[0];

}

static unsigned int getheight(float cx, float cy)
{

    unsigned int mx = cx;
    unsigned int my = cy;

    if (mx < depthimg->w && my < depthimg->h)
    {

        unsigned int mapoffset = my * depthimg->w + mx;

        return depthpixels[mapoffset];
 
    }

    return 0;

}

static void clearfield(unsigned int *pixels)
{

    unsigned int i;

    for (i = 0; i < fieldrect.w * fieldrect.h; i++)
        pixels[i] = colors[0];

}

static void paintpixel(unsigned int *pixels, unsigned int x, unsigned int y, unsigned int w, unsigned int h, float cx, float cy, int height)
{

    int i;

    if (height < 0)
        height = 0;

    for (i = height; i < heightbuffer[x]; i++)
    {

        unsigned int offset = (i * w) + x;

        pixels[offset] = getcolor(cx, cy);

    }

}

void gfx_render(struct camera *camera)
{

    float zfraction = (camera->distance * camera->z);
    float sinphi = sin(camera->angle);
    float cosphi = cos(camera->angle);
    unsigned int *fieldpixels;
    unsigned int x;
    unsigned int y;
    int pitch;
    float plx;
    float ply;
    float prx;
    float pry;
    float dx;
    float dy;
    float cx;
    float cy;
    int rc;

    for (x = 0; x < fieldrect.w; x++)
        heightbuffer[x] = fieldrect.h;

    rc = SDL_LockTexture(field, &fieldrect, (void **)&fieldpixels, &pitch);

    if (rc)
    {

        printf("Lock failed! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

    clearfield(fieldpixels);

    for (y = fieldrect.h + 255; y > 0; y--)
    {

        float z = zfraction / y;

        plx = (-cosphi * z) - (sinphi * z);
        ply = ( sinphi * z) - (cosphi * z);
        prx = ( cosphi * z) - (sinphi * z);
        pry = (-sinphi * z) - (cosphi * z);
        dx = (prx - plx) / (float)fieldrect.w;
        dy = (pry - ply) / (float)fieldrect.w;
        cx = plx + camera->x;
        cy = ply + camera->y;

        for (x = 0; x < fieldrect.w; x++)
        {

            int height = y - getheight(cx, cy) / 2;

            if (height < heightbuffer[x])
            {

                paintpixel(fieldpixels, x, y, fieldrect.w, fieldrect.w, cx, cy, height);

                heightbuffer[x] = height;

            }

            cx += dx;
            cy += dy;

        }

    }

    SDL_UnlockTexture(field);
    SDL_SetRenderDrawColor(renderer, 0x40, 0x80, 0xA0, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, field, &fieldrect, &drect);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);

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

void gfx_init(unsigned int w, unsigned int h)
{

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

    layoutimg = IMG_Load("assets/layout.png");

    if (!layoutimg)
    {

        printf("Layout image could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

    layoutpixels = (unsigned char *)layoutimg->pixels;

    depthimg = IMG_Load("assets/depth.png");

    if (!depthimg)
    {

        printf("Depth image could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

    depthpixels = (unsigned char *)depthimg->pixels;
    field = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);

    if (!field)
    {

        printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);

    }

    skyrect.x = 0;
    skyrect.y = 0;
    skyrect.w = w;
    skyrect.h = h / 8;
    fieldrect.x = 0;
    fieldrect.y = 0;
    fieldrect.w = w;
    fieldrect.h = h - skyrect.h;
    drect.x = 0;
    drect.y = skyrect.h;
    drect.w = w;
    drect.h = fieldrect.h;

}

void gfx_destroy(void)
{

    SDL_DestroyTexture(field);
    SDL_FreeSurface(layoutimg);
    SDL_FreeSurface(depthimg);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

