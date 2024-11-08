#include "board.h"
#include <stdlib.h>
#include <stdio.h>

Board* initBoard()
{
    Board* board = malloc(sizeof(Board));
    for (int i = 0; i < 12; i++) {
        board->houses[i] = 4;
    }
    return board;
}

void displayBoard(Board* board)
{
    for (int i = 0; i < 6; i++) {
        printf("[%d] ", board->houses[i]);
    }
    printf("\n");
    for (int i = 6; i < 12; i++) {
        printf("[%d] ", board->houses[i]);
    }
    printf("\n");
}

int addSeed(Board* board, int houseNb, int seedNb)
{
    if (houseNb >= 0 && houseNb < 12 && seedNb >= 0) 
    {
        board->houses[houseNb] += seedNb;
        return 0;
    }
    return 1;
}

int removeAllSeed(Board* board, int houseNb)
{
    if (houseNb >= 0 && houseNb < 12) 
    {
        board->houses[houseNb] = 0;
        return 0;
    }
    return 1;
}