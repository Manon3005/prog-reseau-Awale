struct Player;
struct Board;

struct Game 
{
    Player player1;
    Player player2;
    Board board;
    int score[2];
    bool clockwise;
    int currentPlayer;
} typedef Game;

Game initGame(Player, Player);
void displayBoard(Game);
void addPointScore(Game, int index, int points);
void chooseHouse(Game, int houseNb);
void attributePoints(Game);
int updateBoard(Game); //retour = nb de points à attribuer au joueur
bool isGameOver(Game);
Player getWinner(Game);



