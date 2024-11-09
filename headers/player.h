typedef struct Player
{
    int id;
    char* username;
    int currentGameId;
    int connectionState;
} Player;

Player* initPlayer(char* username, int id);
int setGame(Player*, int GameId);
void printPlayer(Player*);
