typedef struct Board
{
    int houses[12];
} Board;

void addSeed(Board board, int houseNb, int seedNb);
void removeAllSeed(Board board, int houseNb);
Board initBoard();