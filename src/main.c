#include "../headers/game.h"
#include <stdio.h>

int main(){
    
    char * name0 = "Adri";
    char * name1 = "Man";
    Player* player0 = initPlayer(name0);
    Player* player1 = initPlayer(name1);

    Game* game = initGame(player0, player1);
    printPlayer(game->player0);
    printPlayer(game->player1);

    while(!playOneTurn(game));
    return 0;
}