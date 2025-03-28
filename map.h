struct map
{

    char *layout;
    char *depth;
    unsigned char *layoutpixels;
    unsigned char *depthpixels;

};

void map_init(struct map *map, char *layout, char *depth);
