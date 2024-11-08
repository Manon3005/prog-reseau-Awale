import "game.c";

struct Player
{
    char* username;
    struct Game currentGame;
    bool connectionState;
};
