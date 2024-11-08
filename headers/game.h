#include "board.h"
#include "player.h"

typedef struct Game
{
    int id;
    Player* player0;
    Player* player1;
    Board* board;
    int score[2];
    int clockwise;
    int currentPlayer;
} Game;

Game* initGame(Player*, Player*);
void displayGame(Game*);
void addPointScore(Game*, int index, int points);
int chooseHouse(Game*, int houseNb); // retour = nombre de graines réparties
void attributePoints(Game*);
int updateBoard(Game*); //retour = nb de points à attribuer au joueur
int isGameOver(Game*);
Player getWinner(Game*);



