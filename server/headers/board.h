#ifndef BOARD_H
#define BOARD_H

typedef struct Board
{
    int houses[12];
} Board;

int addSeed(Board* board, int houseNb);
int removeAllSeed(Board* board, int houseNb);
Board* initBoard();
void setBoardTest(Board*, int*);
int getSeedNb(Board* board, int playerNb);

#endif