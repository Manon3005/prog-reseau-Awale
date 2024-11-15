#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"
#include "game.h"

typedef enum {IN_MENU, IN_CHALLENGE_FROM, IN_CHALLENGE_TO, IN_GAME_CURRENT_PLAYER, IN_GAME_WAITING, IN_CONNEXION, IN_REGISTER} State;

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   State state;
   Game* current_game;
} Client;

#endif /* guard */
