#include <stdlib.h>
#include <stdio.h>

#include "../headers/player.h"

Player* initPlayer(char* username){
    Player* player = malloc(sizeof(Player));
    player->username = username;
    player->currentGameId = -1;
    player->connectionState = 0;
    return player;
}


int setGame(Player* player, int gameId){
    if (player->currentGameId == -1){
        player->currentGameId = gameId;
        return 1;
    }
    return 0;
}

void printPlayer(Player* player){
    printf("Player : \n  username : %s\n currentGameId : %d \n connectionState : %d\n", player->username, player->currentGameId, player->connectionState);
}