#include <stdio.h>
#include <stdlib.h>

#include "../../headers/server/game.h"

int main(){ //tester isGameOver
    
    char * name0 = "DD";
    char * name1 = "Man";
    Player* player0 = initPlayer(name0, 1);
    Player* player1 = initPlayer(name1, 2);

    Game* game = initGame(player0, player1, 5);
    printPlayer(game->player[0]);
    printPlayer(game->player[1]);
    
    if (game != NULL) {
        while(!playOneTurn(game));
        player0->currentGameId = -1;
        player1->currentGameId = -1;
        free(game->board);
        free(game);
    }
    free(player0);
    free(player1);
    
    return 0;
}

/*
int main() { //test
    char * name0 = "Adri";
    char * name1 = "Man";
    Player* player0 = initPlayer(name0, 1);
    Player* player1 = initPlayer(name1, 2);

    Game* game = initGame(player0, player1, 5);
    int tab[] = {0,1,0,0,0,0,  1,0,0,0,3,1};
    setBoardTest(game->board, tab);
    displayBoard(game->board, 0);
    game->currentPlayer = 1;
    playOneTurn(game);
    displayGame(game);

    return 0;
}*/