struct Player
{
    char* username;
    struct Game currentGame;
    bool connectionState;
};

Player initPlayer(char* username);
bool setGame(Game);
