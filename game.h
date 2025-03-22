#define GAME_NAME "golf"

unsigned int game_isrunning(void);

void game_moveup(unsigned int press);
void game_movedown(unsigned int press);
void game_moveleft(unsigned int press);
void game_moveright(unsigned int press);
void game_translateheight(float height);
void game_rotateleft(unsigned int press);
void game_rotateright(unsigned int press);
void game_step(void);
void game_stop(void);
void game_init(void);
void game_destroy(void);
