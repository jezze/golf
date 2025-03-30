#include <stdlib.h>
#include "map.h"

float getheight(struct map *map, float cx, float cy)
{

    unsigned int mx = cx;
    unsigned int my = cy;

    if (mx < map->w && my < map->h)
    {

        unsigned int mapoffset = my * map->w + mx;

        return map->depthpixels[mapoffset] - 128;

    }

    return 0;

}

float map_getheight(struct map *map, float cx, float cy)
{

    float x1 = (unsigned int)cx;
    float x2 = (unsigned int)cx + 1;
    float y1 = (unsigned int)cy;
    float y2 = (unsigned int)cy + 1;

    float q11 = getheight(map, x1, y1);
    float q21 = getheight(map, x2, y1);
    float q12 = getheight(map, x1, y2);
    float q22 = getheight(map, x2, y2);

    float r1 = q11 * ((x2 - cx) * (y2 - cy) / (x2 - x1) * (y2 - y1));
    float r2 = q21 * ((cx - x1) * (y2 - cy) / (x2 - x1) * (y2 - y1));
    float r3 = q12 * ((x2 - cx) * (cy - y1) / (x2 - x1) * (y2 - y1));
    float r4 = q22 * ((cx - x1) * (cy - y1) / (x2 - x1) * (y2 - y1));

    return r1 + r2 + r3 + r4;

}

void map_init(struct map *map, char *layout, char *depth, unsigned int w, unsigned int h, unsigned int teex, unsigned int teey)
{

    map->layout = layout;
    map->depth = depth;
    map->w = w;
    map->h = h;
    map->teex = teex;
    map->teey = teey;

}

