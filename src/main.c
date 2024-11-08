#include "../headers/game.h"
#include <stdio.h>

int main(){
    
    char * name0 = "Adri";
    char * name1 = "Man";
    Player* player0 = initPlayer(name0);
    Player* player1 = initPlayer(name1);
    printPlayer(player1);

    Game* game = initGame(player0, player1);
    displayBoard(game->board);
    printf("Last House : %d\n", chooseHouse(game, 4));
    displayBoard(game->board);

    return 0;
}