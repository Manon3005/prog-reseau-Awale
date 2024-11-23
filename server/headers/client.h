#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"
#include "game.h"

typedef enum {IN_MENU, IN_CHALLENGE_FROM, IN_CHALLENGE_TO, IN_GAME_CURRENT_PLAYER, IN_GAME_WAITING, IN_CONNEXION, IN_REGISTER, IN_CHANGING_BIO, IN_CONSULTING_BIO, IN_REWATCHING_GAME, IN_OBSERVE_REQUEST, IN_OBSERVE} State;

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   State state;
   Game* current_game;
} Client;

#endif /* guard */
