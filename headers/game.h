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
    int winner;
} Game;

Game* initGame(Player*, Player*);
void displayGame(Game*);
int chooseHouse(Game*, int houseNb); // retour = nombre de graines réparties
void attributePoints(Game* game, int startHouse, int arrivalHouse);
int playOneTurn(Game*); //retour = nb de points à attribuer au joueur
int isGameOver(Game*);


