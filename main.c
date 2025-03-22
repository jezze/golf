#include <stdlib.h>
#include "game.h"

int main(int argc, char **argv)
{

    game_init();

    while (game_isrunning())
        game_step();

    game_destroy();

    return 0;

}

