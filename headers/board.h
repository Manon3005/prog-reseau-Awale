typedef struct Board
{
    int houses[12];
} Board;

int addSeed(Board* board, int houseNb);
int removeAllSeed(Board* board, int houseNb);
Board* initBoard();
void displayBoard(Board*, int currentPlayer);
void setBoardTest(Board*, int*);
int getSeedNb(Board* board, int playerNb);