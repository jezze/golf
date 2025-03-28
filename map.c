#include <stdlib.h>
#include "map.h"

void map_init(struct map *map, char *layout, char *depth, unsigned int teex, unsigned int teey)
{

    map->layout = layout;
    map->depth = depth;
    map->teex = teex;
    map->teey = teey;

}

