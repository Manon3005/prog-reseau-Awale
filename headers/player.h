struct Player
{
    char* username;
    struct Game currentGame;
    int connectionState;
} typedef Player;

Player initPlayer(char* username);
int setGame(Game);
