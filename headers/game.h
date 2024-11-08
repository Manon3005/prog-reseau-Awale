#include "board.h"
#include "player.h"

typedef struct Game
{
    int id;
    Player* player1;
    Player* player2;
    Board* board;
    int score[2];
    int clockwise;
    int currentPlayer;
} Game;

Game* initGame(Player*, Player*);
void displayGame(Game*);
int chooseHouse(Game*, int houseNb);
void attributePoints(Game*, int startHouse, int houseNb);
int updateBoard(Game*); //retour = nb de points à attribuer au joueur
int isGameOver(Game*);
Player getWinner(Game*);



