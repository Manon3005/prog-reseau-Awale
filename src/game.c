#include "../headers/game.h"
#include <stdlib.h>
#include <stdio.h>

Game* initGame(Player* player0, Player* player1){
    Game* game = malloc(sizeof(Game));
    game->player0 = player0;
    game->player1 = player1;
    game->clockwise = 1;
    game->currentPlayer = 0;
    game->board = initBoard();
    game->winner = NULL;
    return game;
}

int chooseHouse(Game* game, int houseNb){

    int seedNb = removeAllSeed(game->board, houseNb);
    int offset = 1;
    //printf("seedNb = %d", seedNb);
    int i;
    for( i = 0; i < seedNb; i++){
        i = i%11;
        if(houseNb + i + offset == houseNb){
            ++offset;
        }
        addSeed(game->board, houseNb + i + 1);
        
    }
    return houseNb+i;
}; // retour = nombre de graines réparties

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

void displayGame(Game* game) 
{
    printf("Score player1 : %d\n", game->score[0]);
    printf("Score player2 : %d\n", game->score[1]);
    displayBoard(game->board);
}

int isGameOver(Game* game) //à appeler avant le tour
{
    if (game->score[(game->currentPlayer + 1) % 2] >= 25) {
        game->winner = (game->currentPlayer + 1) % 2;
        return 1;
    }
    if () {

    }
    if (false) {
        //remplacer par un moyen de détecter que plus aucune capture n'est possible avec les graines restantes
        return 1;
    }
    return 0;

    int playOneTurn(Game* game)
    {
        displayGame(game);
        int playerChoice;
        int isGood = 0;
        printf("Which house do you choose ?\n")
        while (!isGood) {
            scanf("%d", &playerChoice);
            if (playerChoice > 0 && playerChoice < 7) {
                isGood = 1;
            } else {
                printf("Incorrect choice. Please choose a number between 1 and 6.\n")
            }
        }
        int arrivalHouse;
        if (game->currentPlayer == 0) {
            arrivalHouse = chooseHouse(game, playerChoice);
            attributePoints(game, playerChoice, arrivalHouse);
        } else {
            arrivalHouse = chooseHouse(game, 12 - playerChoice);
            attributePoints(game, 12 - playerChoice, arrivalHouse);
        }
        game->currentPlayer = (game->currentPlayer + 1) % 2;
        return isGameOver(game);
    }
}