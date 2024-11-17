#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../headers/game.h"

int initGame(Game* game, char* player_0, char* player_1)
{
    game->clockwise = 1;
    game->currentPlayer = rand() % 2;
    game->board = initBoard();
    strcpy(game->player[0], player_0);
    strcpy(game->player[1], player_1);
    game->paused = 0;
    return game->currentPlayer;
}

int chooseHouse(Game* game, int houseNb)
{
    int seedNb = removeAllSeed(game->board, houseNb);
    int i;
    int offset = 0;
    for (i = 0; i < seedNb; i++) {
        if( (houseNb + i + offset) % 12 == houseNb){
            ++offset;
        }
        addSeed(game->board, (houseNb + i + offset) % 12);
    }
    return (houseNb + i + offset - 1) % 12;
}

void simulateChoose(Game* game, int houseNb, int* result) {
    //result[0] = nb de graines déposées à l'adversaire et result[1] = nb de graines capturées
    int otherPlayer = (game->currentPlayer + 1) % 2;
    for (int j = 1 ; j <= game->board->houses[houseNb] ; j++) {
        int arrivalHouse = (houseNb + j) % 12;
        if ((arrivalHouse >= otherPlayer * 6) && ((arrivalHouse < (otherPlayer + 1) * 6))) { //si on arrive dans une case adverse
            ++result[0];
             if ((game->board->houses[arrivalHouse] + 1) == 2) {
                result[1] += 2;
            }
            if ((game->board->houses[arrivalHouse] + 1) == 3) {
                result[1] += 3;
            }
        }
    }
}

void attributePoints(Game* game, int startHouse, int arrivalHouse)
{
    Board* board = game->board;
    int pointNb = 0;
    if (game->clockwise) {
        int seedCaptured = 1;
        if (startHouse < 6) {
            while(seedCaptured && arrivalHouse > 5) {
                if (board->houses[arrivalHouse] ==  2 || board->houses[arrivalHouse] ==  3) {
                    pointNb += board->houses[arrivalHouse];
                    removeAllSeed(board, arrivalHouse);
                    arrivalHouse--;
                } else {
                    seedCaptured = 0;
                }
            }
            game->score[0] += pointNb;
        } else {
            while(seedCaptured && arrivalHouse < 6) {
                if (board->houses[arrivalHouse] ==  2 || board->houses[arrivalHouse] ==  3) {
                    pointNb += board->houses[arrivalHouse];
                    removeAllSeed(board, arrivalHouse);
                    arrivalHouse = (arrivalHouse - 1) % 12;
                } else {
                    seedCaptured = 0;
                }
            }
            game->score[1] += pointNb;
        }
    } else {
        //coder en sens inverse
    }
}

int isGameOver(Game* game) //à appeler avant le tour
{
    int otherPlayer = (game->currentPlayer + 1) % 2;

    //Attribution des points en cas de fin par famine ou par indétermination
    if (isThereFamine(game)) {
        game->score[game->currentPlayer] += getSeedNb(game->board, game->currentPlayer);
    } 
    if ((getSeedNb(game->board, game->currentPlayer) + getSeedNb(game->board, otherPlayer)) < 4) {
        game->score[0] += getSeedNb(game->board, 0);
        game->score[0] += getSeedNb(game->board, 1);
    }

    //Vérification des conditions de victoire
    if (game->score[0] == 24 && game->score[1] == 24) {
        printf("Draw ! There is egality between the two players.\n");
        return 1;
    } else if (game->score[0] > 24) {
        game->winner = 0;
        printf("Congratulations to %s who wins the game with %d points\n", game->player[0], game->score[0]);
        return 2;
    } else if (game->score[1] > 24) {
        game->winner = 1;
        printf("Congratulations to %s who wins the game with %d points\n", game->player[1], game->score[1]);
        return ;
    } else {
        return 0; 
    }
}

int isThereFamine(Game* game)
{
    int otherPlayer = (game->currentPlayer + 1) % 2;
    int result[] = {0, 0};
    if (getSeedNb(game->board, otherPlayer) == 0) {
        for (int i = game->currentPlayer * 6 ; i < (game->currentPlayer + 1) * 6 ; i++) {
            simulateChoose(game, i, result);
            if (result[0] > 0) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}