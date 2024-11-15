#ifndef PLAYER_H
#define PLAYER_H

#include "server.h"

typedef struct Player
{
    char username[BUF_SIZE];
    int currentGameId;
    int connectionState;
    SOCKET sock;
    char name[BUF_SIZE];
} Player;

Player* initPlayer(char* username);
int setGame(Player*, int GameId);
void printPlayer(Player*);
