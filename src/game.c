#include "../headers/game.h"
#include <stdlib.h>
#include <stdio.h>

Game* initGame(Player* player0, Player* player1){
    Game* game = malloc(sizeof(Game));
    game->player0 = player0;
    game->player1 = player1;
    game->clockwise = 1;
    game->currentPlayer = 0;
    game->board = initBoard();
    return game;
}

int chooseHouse(Game* game, int houseNb){

    int seedNb = removeAllSeed(game->board, houseNb);
    int offset = 1;
    printf("seedNb = %d", seedNb);
    int i;
    for( i = 0; i < seedNb; i++){
        i = i%11;
        if(houseNb + i + offset == houseNb){
            ++offset;
        }
        addSeed(game->board, houseNb + i + 1);
        
    }

    return houseNb + i;
}; // retour = nombre de graines réparties