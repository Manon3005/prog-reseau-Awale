struct Board
{
    int houses[12];
} typedef Board;

void addSeed(Board board, int houseNb, int seedNb);
void removeAllSeed(Board board, int houseNb);
Board initBoard();