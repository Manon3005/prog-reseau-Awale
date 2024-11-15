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
} Game;

void displayGame(Game*);
int chooseHouse(Game*, int houseNb); // retour = camp d'arrivée
void attributePoints(Game* game, int startHouse, int arrivalHouse);
int playOneTurn(Game*); //retour = 1 si partie finie, 0 sinon
int isGameOver(Game*);
int isThereFamine(Game*);
void simulateChoose(Game*, int houseNb, int* result);

#endif