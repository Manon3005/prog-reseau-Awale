#include "../headers/player.h"

int main(){
    char * name = "Adri";
    Player* player = initPlayer(name);
    printPlayer(player);
    return 0;
}