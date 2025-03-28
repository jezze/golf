struct map
{

    char *layout;
    char *depth;
    unsigned char *layoutpixels;
    unsigned char *depthpixels;
    unsigned int teex;
    unsigned int teey;

};

void map_init(struct map *map, char *layout, char *depth, unsigned int teex, unsigned int teeh);
