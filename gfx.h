#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

void gfx_render(struct camera *camera);
void gfx_input(void);
void gfx_init(unsigned int w, unsigned int h);
void gfx_destroy(void);
