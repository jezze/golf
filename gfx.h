#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_PADDING 24

void gfx_render(struct camera *camera, struct map *map, struct ball *ball);
void gfx_input(void);
void gfx_loadmap(struct map *map);
void gfx_init(unsigned int w, unsigned int h);
void gfx_destroy(void);
