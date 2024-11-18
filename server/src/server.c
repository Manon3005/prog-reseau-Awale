#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "../headers/server.h"
#include "../headers/game.h"
#include "../headers/csvManager.h"

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];

   Request requests[MAX_CLIENTS];
   int actualRequest = 0;

   Game games[MAX_CLIENTS];
   int actualGame = 0;

   fd_set rdfs; //ensemble de descripteurs de fichiers

   csvManager* csvManager;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs); //effacement de l'ensemble

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs); //entree standard ajoutée à l'ensemble

      /* add the connection socket */
      FD_SET(sock, &rdfs); //socket de co ajoutée à l'ensemble

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs); //chaque socket client ajoutée à l'ensemble
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs)) //vrai si le descripteur de fichier est bien dans l'ensemble
      {
         read(STDIN_FILENO, buffer, BUF_SIZE);
         if (strcmp(buffer, "end") == 0) {
            clear_clients(clients, actual);
            end_connection(sock);
            return;
         }
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         printf("new client\n");
         /* new client */
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if (read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         strncpy(c.name, buffer, BUF_SIZE - 1);

         printf("Username : %s\n", buffer);

         int isAlreadyConnected = 0;
         for(int i = 0; i < actual; i++){
            if(strcmp(buffer,clients[i].name) == 0){
               write_client(c.sock, "User already connected. End your previous connection to connect on this device.");
               isAlreadyConnected = 1;
            }
         }
         if(!isAlreadyConnected){
            if (isCLientInCsv(csvManager, buffer)) {
            write_client(c.sock, "Please, enter your password:");
            c.state = IN_CONNEXION;
            } else {
               write_client(csock, "Welcome to our game !\n");
               write_client(csock, "Please create a password:");
               c.state = IN_REGISTER;
            }
            clients[actual] = c;
            actual++;
         }
         printf("actual = %d\n", actual);
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client* client = (&clients[i]);
               int c = read_client(clients[i].sock, buffer);
               printf("read_client : %d\n", c);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  for(int i = 0; i < actualRequest; i++){
                     if(strcmp(requests[i].sender->name, client->name) == 0){
                        write_client(requests[i].receiver->sock, "The request was cancelled because the person who challenged you disconnected");
                        requests[i].receiver->state = IN_MENU;
                        int request_to_remove = get_request_index(requests, actualRequest, client);
                        remove_request(requests, request_to_remove, (&actualRequest));
                     }
                     if(strcmp(requests[i].receiver->name, client->name) == 0){
                        write_client(requests[i].sender->sock, "The request was cancelled because the person you challenged disconnected");
                        requests[i].sender->state = IN_MENU;
                        int request_to_remove = get_request_index(requests, actualRequest, get_sender_from_receiver(requests, actualRequest, client));
                        remove_request(requests, request_to_remove, (&actualRequest));
                     }
                  }
                  for (int i = 0; i < actualGame; i++) {
                     if (strcmp(games[i].player[0], client->name) == 0 || strcmp(games[i].player[1], client->name) == 0 ) {
                        games[i].paused = 1;
                        if (strcmp(games[i].player[0],client->name) == 0){
                           Client* otherPlayerClient = get_client_from_username(clients, actual, games[i].player[1]);
                           if(otherPlayerClient){
                              write_client(otherPlayerClient->sock, "Your opponent disconnected. Please wait for them to reconnect.");
                           }
                        }
                        else{
                           Client* otherPlayerClient = get_client_from_username(clients, actual, games[i].player[0]);
                           if(otherPlayerClient){
                              write_client(otherPlayerClient->sock, "Your opponent disconnected. Please wait for them to reconnect.");
                           }
                        }
                     }
                  }
               remove_client(clients, i, &actual);
               }
               else
               {
                  printf("Buffer : %s\n", buffer);
                  printf("Client state : %d\n", client->state);

                  char d[] = " ";
                  char *p = strtok(buffer, d);
                  char *target;
                  char message[BUF_SIZE];
                  message[0] = 0;
                  if(strcmp(p,"/chat") == 0){
                     target = strtok(NULL, d);
                     Client * client_target;
                     if(target){
                        client_target = is_client_connected(clients, actual, target);
                     }
                     if(client_target){
                        if (strcmp(client_target->name, client->name) == 0){
                           write_client(client_target->sock, "You can't send a message to yourself");
                        }
                        else{
                           strcat(message, "[");
                           strcat(message, client->name);
                           strcat(message, "] ");
                           char * word = strtok(NULL, d);
                           while(word != NULL){
                              strcat(message, word);
                              strcat(message, " ");
                              word = strtok(NULL, d);
                           }
                           write_client(client_target->sock, message);
                        }
                     }
                     else {
                        write_client(client->sock, "This player is not connected");
                     }
                  } else if (client->state == IN_REGISTER) {
                     printf("Password: %s\n", buffer);
                     client->state = IN_MENU;
                     addClientCsv(csvManager, client->name, buffer);
                     write_client(client->sock, "Your account has been created, you are now connected!\n");
                     write_client(client->sock, "You can enter \"menu\" to display the menu.");
                     print_menu(client);
                  } else if (client->state == IN_CONNEXION) {
                     if (authenticateClient(csvManager, client->name, buffer)) {
                        write_client(client->sock, "You are now connected!\n");
                        int wasInGame = 0;
                        for(int i = 0; i < actual; i++){
                           if(strcmp(games[i].player[0], client->name) == 0 || strcmp(games[i].player[1], client->name) == 0 ){
                              wasInGame = 1;
                              client->current_game = &games[i];
                              Client* other_player;
                              if(strcmp(games->player[0], client->name) == 0){
                                 other_player = games->player[1];
                              }
                              else{
                                 other_player = games->player[0];
                              }
                              Client* other_player_connected = is_client_connected(clients, actual, other_player);
                              if(other_player_connected){
                                 client->current_game->paused = 0;
                              }
                              write_client(client->sock, "Reconnecting to the game..." );
                              if((strcmp(games[i].player[games[i].currentPlayer], client->name) == 0 )){
                                 client->state = IN_GAME_CURRENT_PLAYER;
                                 write_client(client->sock, "This is your turn (after reconnection) \n");
                                 print_game(&games[i], client, 1);
                              }
                              else{
                                 client->state = IN_GAME_WAITING;
                                 write_client(client->sock, "Wait for your opponent to play (after reconnection) \n");
                                 print_game(&games[i], client, 0);
                              }
                              if(other_player_connected){
                                 strncpy(buffer, client->name, BUF_SIZE - 1);
                                 strncat(buffer, " reconnected !", BUF_SIZE - strlen(buffer) - 1);
                                 write_client(other_player_connected->sock, buffer);
                              }
                           }
                        }
                        if(!wasInGame){
                           client->state = IN_MENU;
                           write_client(client->sock, "You can enter \"MENU\" to display the menu.");
                           print_menu(client);
                        }
                        
                     } else {
                        write_client(client->sock, "Wrong password, enter your password again:");
                     }
                  } else if (client->state == IN_MENU) {
                     if (strcmp("MENU", buffer) == 0){
                        print_menu(client);
                     } else if (strcmp("1", buffer) == 0){
                        print_online_players(clients, client, actual);
                     } else if (strcmp("2", buffer) == 0){
                        client->state = IN_CHALLENGE_FROM;
                        write_client(client->sock, "Enter the name of the player you want to challenge:");
                     } else if (strcmp("3", buffer) == 0){
                        client->state = IN_CHANGING_BIO;
                        write_client(client->sock, "Your previous bio :");
                        print_bio(csvManager,client->name, client);
                        write_client(client->sock, "\nEnter your new bio :");
                     } else if (strcmp("4", buffer) == 0){
                        client->state = IN_CONSULTING_BIO;
                        write_client(client->sock, "Enter a player username to see their bio:");
                     } else if (strcmp("5", buffer) == 0){
                        print_player_archives(client);
                        print_menu(client);
                     } else if (strcmp("6", buffer) == 0) {
                        client->state = IN_OBSERVE_REQUEST;
                        write_client(client->sock, "Enter the name of the player you want to watch:");
                     } else {
                        write_client(client->sock, "The selected option doesn't exit. Here is the menu:\n");
                        print_menu(client);
                     }
                  } else if (client->state == IN_CHALLENGE_FROM) {
                     if (strcmp(buffer, client->name) != 0) {
                        Client* requested_client = is_client_connected(clients, actual, buffer);
                        if (requested_client) {
                           if (requested_client->state == IN_MENU) {
                              write_client(client->sock, "Request sent to the other player. You can enter \"ABANDON\" to abandon your request.\n");
                              send_request(client, requested_client);
                              Request request = {.sender = client, .receiver = requested_client};
                              requests[actualRequest] = request;
                              actualRequest++;
                              client->state = IN_CHALLENGE_WAIT;
                           } else if (requested_client->state == IN_GAME_CURRENT_PLAYER || requested_client->state == IN_GAME_WAITING) {
                              write_client(client->sock, "This player is already playing a game. Try later.\n");
                              client->state = IN_MENU;
                              print_menu(client);
                           } else {
                              write_client(client->sock, "This player is already involved in a game request. Try later.\n");
                              client->state = IN_MENU;
                              print_menu(client);
                           } 
                        } else {
                           write_client(client->sock, "This player isn't connected.\n");
                           client->state = IN_MENU;
                           print_menu(client);
                        }
                     } else {
                        write_client(client->sock, "You can't challenge yourself.\n");
                     }
                  } else if (client->state == IN_CHALLENGE_WAIT) {
                     if (strcmp(buffer, "ABANDON") == 0) {
                        Client* client_receiver = get_receiver_from_sender(requests, actualRequest, client);

                        int request_to_remove = get_request_index(requests, actualRequest, client);
                        remove_request(requests, request_to_remove, (&actualRequest));

                        write_client(client_receiver->sock, client->name);
                        write_client(client_receiver->sock, " abandoned its game request. You're back in the menu.\n");
                        client_receiver->state = IN_MENU;
                        client->state = IN_MENU;
                        write_client(client->sock, "You're back in the menu.\n");
                     } else {
                        write_client(client->sock, "Wait for the other player to answer your request.\n");
                     }
                  } else if (client->state == IN_CHALLENGE_TO) {
                     Client* client_sender = get_sender_from_receiver(requests, actualRequest, client);
                     if (strcmp("Y", buffer) == 0) {
                        client->state = IN_GAME_CONFIG_WAIT;
                        client_sender->state = IN_GAME_CONFIG;
                        write_client(client->sock, "Wait for your opponent to configure the game.\n");
                        write_client(client_sender->sock, "Do you want to make your game private (Y) or not (N) ?\n");
                     } else if (strcmp("N", buffer) == 0) {
                        client->state = IN_MENU;
                        client_sender->state = IN_MENU;
                        write_client(client->sock, "Request refused.\n");
                        write_client(client_sender->sock, "Request refused.\n");
                        print_menu(client);
                        print_menu(client_sender);
                        int request_to_remove = get_request_index(requests, actualRequest, client_sender);
                        remove_request(requests, request_to_remove, (&actualRequest));
                     } else {
                        write_client(client->sock, "Wrong answer. Enter \"Y\" or \"N\" please.\n");
                     }
                  } else if (client->state == IN_GAME_CONFIG) {
                     int private = -1;
                     if (strcmp(buffer, "Y") == 0) {
                        private = 1;
                     } else if (strcmp(buffer, "N") == 0) {
                        private = 0;
                     }
                     if (private > -1) {
                        Client* client_receiver = get_receiver_from_sender(requests, actualRequest, client);
                        write_client(client->sock, "Starting of the game.\n");
                        write_client(client_receiver->sock, "Request accepted. Starting of the game.\n");

                        int request_to_remove = get_request_index(requests, actualRequest, client);
                        remove_request(requests, request_to_remove, (&actualRequest));

                        Game new_game;
                        int current_player = initGame(&new_game, client->name, client_receiver->name);
                        new_game.is_private = private;
                        games[actualGame] = new_game;
                        client->current_game = (&games[actualGame]);
                        client_receiver->current_game = (&games[actualGame]);
                        actualGame++;

                        if (current_player == 0) {
                           print_game(&new_game, client, 1);
                           write_client(client->sock, "Which house do you choose ?\n");
                           write_client(client_receiver->sock, "The other play starts. Wait for him to choose a house.\n");
                           client_receiver->state = IN_GAME_WAITING;
                           client->state = IN_GAME_CURRENT_PLAYER;
                        } else {
                           print_game(&new_game, client_receiver, 1);
                           write_client(client_receiver->sock, "Which house do you choose ?\n");
                           write_client(client->sock, "The other player starts. Wait for him to choose a house.\n");
                           client_receiver->state = IN_GAME_CURRENT_PLAYER;
                           client->state = IN_GAME_WAITING;
                        }
                     } else {
                        write_client(client->sock, "Wrong answer. Enter \"Y\" or \"N\" please.\n");
                     }
                  } else if (client->state == IN_GAME_CONFIG_WAIT) {
                     write_client(client->sock, "Wait for your opponent to configure the game.\n");
                  } else if (client->state == IN_GAME_CURRENT_PLAYER) {
                     Game* game = client->current_game;
                     Client* otherPlayerClient = get_client_from_username(clients, actual, game->player[(((game->currentPlayer)+1)%2)]);
                     if(!game->paused){
                        int playerChoice = atoi(buffer);
                        int isGood = 0;
                        int result[] = {0, 0};
                        int otherPlayer = (game->currentPlayer + 1) % 2;
                        int houseNb;
                        if (playerChoice > 0 && playerChoice < 7) {
                              if (game->currentPlayer == 0) {
                                 houseNb = playerChoice - 1;
                              } else {
                                 houseNb = 12 - playerChoice;
                              }
                              if (game->board->houses[houseNb] == 0) {
                                 write_client(client->sock, "You must choose a house with at least one seed in it. Please select another house.\n");
                              } else {
                                 simulateChoose(game, houseNb, result);
                                 if (getSeedNb(game->board, otherPlayer) == 0 && result[0] == 0) {
                                    write_client(client->sock, "You can't starve your opponent. Please select another house.\n");
                                 } else {
                                    isGood = 1;
                                 }
                              }
                        } else if (strcmp(buffer, "ABANDON") == 0){
                           otherPlayerClient->state = IN_SAVING_GAME;
                           client->state = IN_SAVING_GAME;
                           game->winner = (game->currentPlayer + 1) % 2;
                           char newMoves[10];
                           newMoves[0] = 0;
                           strcat(newMoves, buffer);
                           strcat(newMoves, " ");
                           strcpy(game->moves, &newMoves);
                           print_game_end(game, 2, client, otherPlayerClient);
                        }
                        else {
                           write_client(client->sock, "Incorrect choice. Please choose a number between 1 and 6 or abandon.\n");
                        }
                        if (isGood) {
                           int arrivalHouse = chooseHouse(game, houseNb);
                           char newMoves [10];
                           strcat(newMoves, buffer);
                           strcat(newMoves, " ");
                           strcpy(game->moves, &newMoves);
                           if (result[1] != getSeedNb(game->board, otherPlayer)) {
                              attributePoints(game, houseNb, arrivalHouse);
                           } else {
                              write_client(client->sock, "No capture because your opponent wouldn't have any seed left otherwise.\n");
                           }
                           game->currentPlayer = (game->currentPlayer + 1) % 2;
                           int is_game_over = isGameOver(game);
                           write_client(otherPlayerClient->sock, "Your opponent just played, here is the result:\n");
                           print_game(game, otherPlayerClient, 1);
                           write_client(client->sock, "Thanks for your choice. Here is the result:\n");
                           print_game(game, client, 0);
                           for (int i = 0 ; i < game->nb_observer ; i++) {
                              Client* observer_client = is_client_connected(clients, actual, game->observer[i]);
                              if (observer_client) {
                                 write_client(observer_client->sock, client->name);
                                 write_client(observer_client->sock, " just played, here is the result:\n");
                                 print_game(game, observer_client, 0);
                              } else {
                                 remove_observer(game, observer_client->name);
                              }
                           }
                           if (is_game_over == 0) {
                              otherPlayerClient->state = IN_GAME_CURRENT_PLAYER;
                              client->state = IN_GAME_WAITING;
                              write_client(otherPlayerClient->sock, "It's your turn to play! Which house do you choose ?\n");
                              write_client(client->sock, "Now, wait for your opponent to play.\n");
                           } else {
                              otherPlayerClient->state = IN_SAVING_GAME;
                              client->state = IN_SAVING_GAME;
                              print_game_end(game, is_game_over, client, otherPlayerClient);
                           }
                        }
                     }
                     else{
                        write_client(client->sock,"Wait for you opponent to reconnect.");
                     }
                  } else if (client->state == IN_GAME_WAITING) {
                     write_client(client->sock, "It's not your turn to play. Please, wait for the other player!");
                  } else if (client->state == IN_CHANGING_BIO) {
                     int bioChanged = changeBioCsv(csvManager, client->name, buffer);
                     if (bioChanged){
                        write_client(client->sock, "Bio changed. Returning to menu\n");
                     } else{
                        write_client(client->sock, "A problem happened. Please try again later.");
                     }
                     print_menu(client);
                     client->state = IN_MENU;
                  } else if (client->state == IN_CONSULTING_BIO){
                     if(strcmp(buffer,"MENU") == 0){
                        client->state = IN_MENU;
                        print_menu(client);
                     }
                     else{
                        print_bio(csvManager,buffer, client);
                        write_client(client->sock, "\nEnter another username to see their bio. Enter 'MENU' to come back to the menu when you are done.");
                     }
                  } else if (client->state == IN_SAVING_GAME) {
                     if (strcmp("Y", buffer) == 0){
                        char date [20];
                        getCurrentDateTime(&date, sizeof(date));
                        addGameToCsv(csvManager, client->current_game->player[0], client->current_game->player[1], client->current_game->moves, &date);
                        client->current_game = NULL;
                        client->current_game = NULL;
                        client->state = IN_MENU;
                        print_menu(client);
                     } else if (strcmp("N", buffer) == 0){
                        client->current_game = NULL;
                        client->state = IN_MENU;
                        print_menu(client);
                     }
                     else{
                        write_client(client->sock, "Wrong answer. Enter \"Y\" or \"N\" please.\n");
                     }
                  } else if (client->state == IN_OBSERVE_REQUEST) {
                     if (strcmp(buffer, client->name) != 0) {
                        Client* requested_client = is_client_connected(clients, actual, buffer);
                        if (requested_client) {
                           if (requested_client->state == IN_GAME_CURRENT_PLAYER || requested_client->state == IN_GAME_WAITING) {
                              if (!requested_client->current_game->is_private) {
                                 if (add_observer(requested_client->current_game, client->name)) {
                                 client->state = IN_OBSERVE;
                                 client->current_game = requested_client->current_game;
                                 write_client(client->sock, "Now, you observe the game.\n");
                                 write_client(client->sock, "Enter \"MENU\" to get back to the menu.\n");
                                 } else {
                                    write_client(client->sock, "You can't observe this game, too many players are already watching it.\n");
                                 }
                              } else {
                                 write_client(client->sock, "You can't observe this game, it's a private game.\n");
                              }
                           } else {
                              write_client(client->sock, "This player isn't playing any game.\n");
                              client->state = IN_MENU;
                              print_menu(client);
                           }
                        } else {
                           write_client(client->sock, "This player isn't connected.\n");
                           client->state = IN_MENU;
                           print_menu(client);
                        }
                     }
                  } else if (client->state == IN_OBSERVE) {
                     if (strcmp(buffer, "MENU") == 0) {
                        client->state = IN_MENU;
                        print_menu(client);
                        remove_observer(client->current_game, client->name);
                        client->current_game = NULL;
                     }
                  }
               }
               break;
            }
         }
      }
   }
}

void print_player_archives(Client* client){
   SavedGame* games = NULL;
   int gameCount = 0;

   if (getGamesByPlayer(client->name, &games, &gameCount)) {
      char message[BUF_SIZE];
      message[0] = 0;
      char str[10];
      strcat(message, "Number of games : ");
      intToStr(gameCount, str, 10);
      strcat(message, str);
      strcat(message, "\n");
      for (int i = 0; i < gameCount; i++) {
         strcat(message, "Game ");
         intToStr(i + 1, str, 10);
         strcat(message, str);
         strcat(message, " : ");
         strcat(message, games[i].player1);
         strcat(message, " vs ");
         strcat(message, games[i].player2);
         strcat(message, "  Date: ");
         strcat(message, games[i].date);
         strcat(message, "\n");
      }
      write_client(client->sock, message);
      free(games);
   } else {
      write_client(client->sock, "No game saved");
   }
}

static void print_game_end(Game* game, int status, Client* player_0, Client* player_1) {
   if (status == 1) {
      write_client(player_0->sock, "Draw ! There is egality between the two players.\n");
      write_client(player_1->sock, "Draw ! There is egality between the two players.\n");
   } else if (status == 2){
      char message[BUF_SIZE];
      char str[10];
      message[0] = 0;
      strcat(message, "Congratulations to ");
      strcat(message, game->player[game->winner]);
      strcat(message, " who wins the game by abandon of their opponent"); 
      write_client(player_0->sock, message);
      write_client(player_1->sock, message);
   }
   else {
      char message[BUF_SIZE];
      char str[10];
      message[0] = 0;
      strcat(message, "Congratulations to ");
      strcat(message, game->player[game->winner]);
      strcat(message, " who wins the game with ");
      sprintf(str, "%d", game->score[game->winner]);
      strcat(message, str);
      strcat(message, " points.\n");
      write_client(player_0->sock, message);
      write_client(player_1->sock, message);
   }
   write_client(player_0->sock, "Do you want to save the game ? (Y/N)");
   write_client(player_1->sock, "Do you want to save the game ? (Y/N)");
}

static Client* get_client_from_username(Client* clients, int actual, char* username) {
   for (int i = 0 ; i < actual ; i++) {
      if (strcmp(clients[i].name, username) == 0) {
         return (&clients[i]);
      }
   }
   return NULL;
}

static Client* get_sender_from_receiver(Request* requests, int actual, Client* receiver) {
   for (int i = 0 ; i < actual ; i++) {
      if (requests[i].receiver ==  receiver) {
         return (requests[i].sender);
      }
   }
   return NULL;
}

static Client* get_receiver_from_sender(Request* requests, int actual, Client* sender) {
   for (int i = 0 ; i < actual ; i++) {
      if (requests[i].sender ==  sender) {
         return (requests[i].receiver);
      }
   }
   return NULL;
}

static void print_game(Game* game, Client* client, int in_choice) 
{
   char str[10];
   char message[BUF_SIZE];
   message[0] = 0;
   write_client(client->sock, "--------------------------------------\n");
   print_board(game, client, in_choice);
   strcat(message, "Score ");
   strcat(message, game->player[0]);
   strcat(message, ": ");
   sprintf(str, "%d", game->score[0]);
   strcat(message, str);
   strcat(message, "\n");
   strcat(message, "Score ");
   strcat(message, game->player[1]);
   strcat(message, ": ");
   sprintf(str, "%d", game->score[1]);
   strcat(message, str);
   strcat(message, "\n");
   strcat(message, "--------------------------------------\n");
   write_client(client->sock, message);
}

static void print_board(Game* game, Client* client, int in_choice)
{
   char message[BUF_SIZE];
   message[0] = 0;
   char str[10];
   for (int i = 0 ; i < 6 ; i++) {
      sprintf(str, "%d", game->board->houses[i]);
      strcat(message, "[");
      strcat(message, str);
      strcat(message, "] ");
   }
   if (in_choice && game->currentPlayer == 0) {
      strcat(message, "  <- your side");
   }
   strcat(message, "\n");
   for (int i = 11; i > 5 ; i--) {
      sprintf(str, "%d", game->board->houses[i]);
      strcat(message, "[");
      strcat(message, str);
      strcat(message, "] ");
   }
   if (in_choice && game->currentPlayer == 1) {
      strcat(message, "  <- your side");
   }
   strcat(message, "\n");
   write_client(client->sock, message);
}


static void send_request(Client* sender, Client* receiver) {
   write_client(receiver->sock, sender-> name);
   write_client(receiver->sock, " wants to play with you ? Do you accept (Y) or deny (N) ?\n");
   receiver->state = IN_CHALLENGE_TO;
}

static Client* is_client_connected(Client *clients, int actual, char* username) {
   for (int i = 0 ; i < actual ; i++) {
      if (strcmp(clients[i].name, username) == 0) {
         return (&clients[i]);
      }
   }
   return NULL;
}

static void print_online_players(Client *clients, Client *client, int actual) {
   write_client(client->sock, "Online players:\n");
   for (int i = 0 ; i < actual ; i++) {
      write_client(client->sock, "- ");
      write_client(client->sock, clients[i].name);
      write_client(client->sock, "\n");
   }
}

static void print_menu(Client *client) {
   write_client(client->sock, "Please select an option.\n");
   write_client(client->sock, "[1] Online players list\n");
   write_client(client->sock, "[2] Challenge a player\n");
   write_client(client->sock, "[3] Change your bio\n");
   write_client(client->sock, "[4] Read a player's bio\n");
   write_client(client->sock, "[5] See your games' archives\n");
   write_client(client->sock, "[6] Observe a game\n");
   write_client(client->sock, "Option selected:");
}

void print_bio(csvManager* csvManager, char* username, Client* receiver){
   char * bio = getBioFromCsv(csvManager, username);
   if(bio){
      write_client(receiver->sock, bio);
   }
   else{
      write_client(receiver->sock, "This player doesn't exist.");
   }
}

void intToStr(int value, char* buffer, size_t size) {
    snprintf(buffer, size, "%d", value); // Convertit 'value' en chaîne
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static int get_request_index(Request *requests, int actual, Client* sender) {
   for(int i = 0; i < actual; i++)
   {
      if (requests[i].sender == sender) {
         return i;
      }
   }
   return -1;
}

static void remove_request(Request *requests, int to_remove, int *actual)
{
   /* we remove the request in the array */
   memmove(requests + to_remove, requests + to_remove + 1, (*actual - to_remove - 1) * sizeof(Request));
   /* number request - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if (from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   printf("receive\n");
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
