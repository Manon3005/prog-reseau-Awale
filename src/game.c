#include "../headers/game.h"
#include <stdlib.h>
#include <stdio.h>


void attributePoints(Game* game, int startHouse, int houseNb)
{
    Board* board = game->board;
    int pointNb = 0;
    if (game->clockwise) {
        int arrivalHouse = (startHouse + houseNb) % 12;
        int seedCaptured = 1;
        if (startHouse < 6) {
            while(seedCaptured && arrivalHouse > 5) {
                if (board->houses[arrivalHouse] ==  2 || board->houses[arrivalHouse] ==  3) {
                    pointNb += board->houses[arrivalHouse];
                    removeAllSeed(board, arrivalHouse);
                    arrivalHouse--;
                } else {
                    seedCaptured = 0;
                }
                game->score[0] += pointNb;
            }
        } else {
            while(seedCaptured && arrivalHouse < 6) {
                if (board->houses[arrivalHouse] ==  2 || board->houses[arrivalHouse] ==  3) {
                    pointNb += board->houses[arrivalHouse];
                    removeAllSeed(board, arrivalHouse);
                    arrivalHouse = (arrivalHouse - 1) % 12;
                } else {
                    seedCaptured = 0;
                }
                game->score[1] += pointNb;
            }
        }
    } else {
        //coder en sens inverse
    }
}

void displayGame(Game* game) 
{
    printPlayer(game->player1);
    printPlayer(game->player2);
    displayBoard(game->board);
    printf("Score player1 : %d\n", game->score[0]);
    printf("Score player2 : %d\n", game->score[1]);
}