#ifndef GAME_H
#define GAME_H

#include "board.h"

typedef struct Game
{
    char player[2][1024];
    Board* board;
    int score[2];
    int clockwise;
    int currentPlayer;
    int winner;
    int paused;
    char moves[1024];
    char observer[10][1024];
    int nb_observer;
    int is_private;
    int quitting_allowed;
} Game;

int initGame(Game* game, char* player_0, char* player_1);
int chooseHouse(Game*, int houseNb); // retour = camp d'arrivée
void attributePoints(Game* game, int startHouse, int arrivalHouse);
int isGameOver(Game*);
int isThereFamine(Game*);
void simulateChoose(Game*, int houseNb, int* result);
int add_observer(Game* game, char* username);
void remove_observer(Game* game, char* username);

#endif