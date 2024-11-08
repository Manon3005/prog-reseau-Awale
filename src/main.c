#include "../headers/game.h"
#include <stdio.h>

int main(){
    
    char * name0 = "Adri";
    char * name1 = "Man";
    Player* player0 = initPlayer(name0);
    Player* player1 = initPlayer(name1);

    Game* game = initGame(player0, player1);
    displayGame(game);
    int arrivalHouse = chooseHouse(game, 5);
    displayGame(game);
    int tab[12] = {4,4,4,4,4,4,2,4,3,3,4,4};
    setBoardTest(game->board, tab);
    displayGame(game);
    attributePoints(game, 5, arrivalHouse);
    displayGame(game);
    return 0;
}