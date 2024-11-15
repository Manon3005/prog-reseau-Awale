typedef struct Player
{
    char* username;
    int currentGameId;
    int connectionState;
} Player;

Player* initPlayer(char* username);
int setGame(Player*, int GameId);
void printPlayer(Player*);