#include "../headers/game.h"
#include <stdlib.h>
#include <stdio.h>

Game* initGame(Player* player0, Player* player1, int id)
{
    if (id < 0) {
        printf("Game Id should be > 0.\n");
        return NULL;
    }
    if (!setGame(player0, id)){
        printf("%s is already playing.\n", player0->username);
        return NULL;
    }
    if (!setGame(player1, id)){
        player0->currentGameId = -1;
        printf("%s is already playing.\n", player1->username);
        return NULL;
    }
    Game* game = malloc(sizeof(Game));
    game->player[0] = player0;
    game->player[1] = player1;
    game->clockwise = 1;
    game->currentPlayer = 0;
    game->board = initBoard();
    game->id = id;
    return game;
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

void displayGame(Game* game) 
{
    printf("\n");
    printf("--------------------------------------\n");
    displayBoard(game->board, game->currentPlayer);
    printf("\n");
    printf("Score %s : %d\n", game->player[0]->username, game->score[0]);
    printf("Score %s : %d\n", game->player[1]->username, game->score[1]);
    printf("--------------------------------------\n");
    printf("\n");
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
    } else if (game->score[0] > 25) {
        game->winner = 0;
        printf("Congratulations to %s who wins the game with %d points\n", game->player[0]->username, game->score[0]);
        return 1;
    } else if (game->score[1] > 25) {
        game->winner = 1;
        printf("Congratulations to %s who wins the game with %d points\n", game->player[1]->username, game->score[1]);
        return 1;
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

int playOneTurn(Game* game)
{
    displayGame(game);
    int playerChoice;
    int isGood = 0;
    int result[] = {0, 0};
    int otherPlayer = (game->currentPlayer + 1) % 2;
    int houseNb;
    printf("%s, which house do you choose ?\n", game->player[game->currentPlayer]->username);
    while (!isGood) {
        scanf("%d", &playerChoice);
        if (playerChoice > 0 && playerChoice < 7) {
            if (game->currentPlayer == 0) {
                houseNb = playerChoice - 1;
            } else {
                houseNb = 12 - playerChoice;
            }
            if (game->board->houses[houseNb] == 0) {
                printf("You must choose a house with at least one seed in it. Please select another house.\n");
            } else {
                simulateChoose(game, houseNb, result);
                if (getSeedNb(game->board, otherPlayer) == 0 && result[0] == 0) {
                    printf("You can't starve your opponent. Please select another house.\n");
                } else {
                    isGood = 1;
                }
            }
        } else {
            printf("Incorrect choice. Please choose a number between 1 and 6.\n");
        }
    }
    int arrivalHouse = chooseHouse(game, houseNb);
    if (result[1] != getSeedNb(game->board, otherPlayer)) {
        attributePoints(game, houseNb, arrivalHouse);
    } else {
        printf("No capture because your opponent wouldn't have any seed left otherwise.\n");
    }
    game->currentPlayer = (game->currentPlayer + 1) % 2;
    return isGameOver(game);
}