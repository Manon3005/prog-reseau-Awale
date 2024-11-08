typedef struct Board
{
    int houses[12];
} Board;

int addSeed(Board* board, int houseNb, int seedNb);
int removeAllSeed(Board* board, int houseNb);
Board* initBoard();
void displayBoard(Board*);