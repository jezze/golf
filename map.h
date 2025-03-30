#define MAP_TYPE_NONE 0
#define MAP_TYPE_FAIRWAY 1
#define MAP_TYPE_GREEN 2
#define MAP_TYPE_ROUGH 3
#define MAP_TYPE_DEEPROUGH 4
#define MAP_TYPE_SAND 5
#define MAP_TYPE_WATER 6
#define MAP_TYPE_HOLE 7

struct map
{

    char *layout;
    char *depth;
    unsigned char *layoutpixels;
    unsigned char *depthpixels;
    unsigned int w;
    unsigned int h;
    unsigned int teex;
    unsigned int teey;

};

int map_getheight(struct map *map, float cx, float cy);
void map_init(struct map *map, char *layout, char *depth, unsigned int w, unsigned int h, unsigned int teex, unsigned int teeh);
