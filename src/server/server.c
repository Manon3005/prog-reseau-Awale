#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "../../headers/server/server.h"
#include "../../headers/server/player.h"

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
   Player players[MAX_CLIENTS];

   fd_set rdfs; //ensemble de descripteurs de fichiers

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
         FD_SET(players[i].sock, &rdfs); //chaque socket client ajoutée à l'ensemble
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs)) //vrai si le descripteur de fichier est bien dans l'ensemble ?
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         printf("new\n");
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
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Player c = { csock };
         strncpy(c.name, buffer, BUF_SIZE - 1);
         printf("name : %s", c.name);
         players[actual] = c;
         actual++;
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(players[i].sock, &rdfs))
            {
               Player player = players[i];
               int c = read_client(players[i].sock, buffer);
               printf("%s\n", c);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(players[i].sock);
                  remove_client(players, i, &actual);
                  strncpy(buffer, player.username, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(players, player, actual, buffer, 1);
               }
               else
               {
                  send_message_to_all_clients(players, player, actual, buffer, 0);
               }
               break;
            }
         }
      }
   }

   clear_clients(players, actual);
   end_connection(sock);
}

static void clear_clients(Player *players, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(players[i].sock);
   }
}

static void remove_client(Player *players, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(players + to_remove, players + to_remove + 1, (*actual - to_remove - 1) * sizeof(Player));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Player *players, Player sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != players[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(players[i].sock, message);
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
