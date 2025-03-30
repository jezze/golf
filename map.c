#include <stdlib.h>
#include "map.h"

int map_getheight(struct map *map, float cx, float cy)
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

void map_init(struct map *map, char *layout, char *depth, unsigned int w, unsigned int h, unsigned int teex, unsigned int teey)
{

    map->layout = layout;
    map->depth = depth;
    map->w = w;
    map->h = h;
    map->teex = teex;
    map->teey = teey;

}

