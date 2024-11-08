#include "board.h"

int main(){
    Board* board = initBoard();
    displayBoard(board);
    addSeed(board, 0, 5);
    displayBoard(board);
    removeAllSeed(board, 7);
    displayBoard(board);
    removeAllSeed(board, 12);
    addSeed(board, -1, 0);
    displayBoard(board);
    return 0;
}