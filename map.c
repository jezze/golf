#include <stdlib.h>
#include "map.h"

void map_init(struct map *map, char *layout, char *depth)
{

    map->layout = layout;
    map->depth = depth;

}

