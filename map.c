#include <stdlib.h>
#include <math.h>
#include "map.h"

float getheight(struct map *map, unsigned int mx, unsigned int my)
{

    if (mx < map->w && my < map->h)
    {

        unsigned int mapoffset = my * map->w + mx;

        return map->depthpixels[mapoffset] - 128;

    }

    return 0;

}

float map_getheight(struct map *map, float cx, float cy)
{

    return getheight(map, cx, cy);

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

