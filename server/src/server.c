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
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client->name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, (*client), actual, buffer, 1);
               }
               else
               {
                  printf("Buffer : %s\n", buffer);
                  printf("Client state : %d\n", client->state);
                  if (client->state == IN_REGISTER) {
                     printf("Password: %s\n", buffer);
                     client->state = IN_MENU;
                     addClientCsv(csvManager, client->name, buffer);
                     write_client(client->sock, "Your account has been created, you are now connected!\n");
                     write_client(client->sock, "You can enter \"menu\" to display the menu.");
                     print_menu(client);
                  } else if (client->state == IN_CONNEXION) {
                     if (authenticateClient(csvManager, client->name, buffer)) {
                        client->state = IN_MENU;
                        addClientCsv(NULL, client->name, buffer);
                        write_client(client->sock, "You are now connected!\n");
                        write_client(client->sock, "You can enter \"menu\" to display the menu.");
                        print_menu(client);
                     } else {
                        write_client(client->sock, "Wrong password, enter your password again:");
                     }
                  } else if (client->state == IN_MENU) {
                     if (strcmp("menu", buffer) == 0){
                        print_menu(client);
                     } else if (strcmp("1", buffer) == 0){
                        print_online_players(clients, client, actual);
                     } else if (strcmp("2", buffer) == 0){
                        client->state = IN_CHALLENGE_FROM;
                        write_client(client->sock, "Enter the name of the player you want to challenge:");
                     }
                  } else if (client->state == IN_CHALLENGE_FROM) {
                     if (strcmp(buffer, client->name) != 0) {
                        Client* requested_client = is_client_connected(clients, actual, buffer);
                        if (requested_client) {
                           if (requested_client->state == IN_MENU) {
                              write_client(client->sock, "Request sent to the other player.\n");
                              send_request(client, requested_client);
                              Request request = {client, requested_client};
                              requests[actualRequest] = request;
                              actualRequest++;
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
                  } else if (client->state == IN_CHALLENGE_TO) {
                     Client* client_sender = get_sender_from_receiver(requests, actualRequest, client);
                     if (strcmp("Y", buffer) == 0) {
                        write_client(client->sock, "Starting of the game.\n");
                        write_client(client_sender->sock, "Request accepted. Starting of the game.\n");

                        int request_to_remove = get_request_index(requests, actualRequest, client_sender);
                        remove_request(requests, request_to_remove, (&actualRequest));

                        Game new_game;
                        int current_player = initGame(&new_game, client_sender->name, client->name);
                        games[actualGame] = new_game;
                        client->current_game = (&games[actualGame]);
                        client_sender->current_game = (&games[actualGame]);
                        actualGame++;

                        if (current_player == 0) {
                           print_game(&new_game, client_sender);
                           write_client(client_sender->sock, "Which house do you choose ?\n");
                           write_client(client->sock, "The other play starts. Wait for him to choose a house.\n");
                           client->state = IN_GAME_WAITING;
                           client_sender->state = IN_GAME_CURRENT_PLAYER;
                        } else {
                           print_game(&new_game, client);
                           write_client(client->sock, "Which house do you choose ?\n");
                           write_client(client_sender->sock, "The other player starts. Wait for him to choose a house.\n");
                           client->state = IN_GAME_CURRENT_PLAYER;
                           client_sender->state = IN_GAME_WAITING;
                        }
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
                  } else if (client->state == IN_GAME_CURRENT_PLAYER) {
                     Game* game = client->current_game;
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
                     } else {
                           write_client(client->sock, "Incorrect choice. Please choose a number between 1 and 6.\n");
                     }
                     if (isGood) {
                        int arrivalHouse = chooseHouse(game, houseNb);
                        if (result[1] != getSeedNb(game->board, otherPlayer)) {
                           attributePoints(game, houseNb, arrivalHouse);
                        } else {
                           write_client(client->sock, "No capture because your opponent wouldn't have any seed left otherwise.\n");
                        }
                        game->currentPlayer = (game->currentPlayer + 1) % 2;
                        Client* otherPlayerClient = get_client_from_username(clients, actual, game->player[game->currentPlayer]);
                        int is_game_over = isGameOver(game);
                        write_client(otherPlayerClient->sock, "Your opponent just played, here is the result:\n");
                        print_game(game, otherPlayerClient);
                        write_client(client->sock, "Thanks for your choice. Here is the result:\n");
                        print_game(game, client);
                        if (is_game_over == 0) {
                           otherPlayerClient->state = IN_GAME_CURRENT_PLAYER;
                           client->state = IN_GAME_WAITING;
                           write_client(otherPlayerClient->sock, "It's your turn to play! Which house do you choose ?\n");
                           write_client(client->sock, "Now, wait for your opponent to play.\n");
                        } else {
                           otherPlayerClient->state = IN_MENU;
                           client->state = IN_MENU;
                           otherPlayerClient->current_game = NULL;
                           client->current_game = NULL;
                           print_game_end(game, is_game_over, client, otherPlayerClient);
                           print_menu(client);
                           print_menu(otherPlayerClient);
                        }
                     }
                  } else if (client->state == IN_GAME_WAITING) {
                     write_client(client->sock, "It's not your turn to play. Please, wait for the other player!");
                  }
               }
               break;
            }
         }
      }
   }
}

static void print_game_end(Game* game, int status, Client* player_0, Client* player_1) {
   if (status == 1) {
      write_client(player_0->sock, "Draw ! There is egality between the two players.\n");
      write_client(player_1->sock, "Draw ! There is egality between the two players.\n");
   } else {
      char message[BUF_SIZE];
      char str[2];
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

static void print_game(Game* game, Client* client) 
{
   char str[2];
   char message[BUF_SIZE];
   message[0] = 0;
   write_client(client->sock, "--------------------------------------\n");
   print_board(game, client);
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

static void print_board(Game* game, Client* client)
{
   char message[BUF_SIZE];
   message[0] = 0;
   char str[2];
   for (int i = 0 ; i < 6 ; i++) {
      sprintf(str, "%d", game->board->houses[i]);
      strcat(message, "[");
      strcat(message, str);
      strcat(message, "] ");
   }
   if (game->currentPlayer == 0) {
      strcat(message, "  <- your side");
   }
   strcat(message, "\n");
   for (int i = 11; i > 5 ; i--) {
      sprintf(str, "%d", game->board->houses[i]);
      strcat(message, "[");
      strcat(message, str);
      strcat(message, "] ");
   }
   if (game->currentPlayer == 1) {
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
   write_client(client->sock, "Option selected:");
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
