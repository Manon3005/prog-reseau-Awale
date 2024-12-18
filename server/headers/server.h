#ifndef SERVER_H
#define SERVER_H

#ifdef WIN32

#include <winsock2.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF        "\r\n"
#define PORT         1977
#define MAX_CLIENTS     100

#define BUF_SIZE    1024

#include "client.h"
#include "request.h"

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client* clients, Client client, int actual, const char *buffer, char from_server);
static void remove_client(Client* clients, int to_remove, int *actual);
static void clear_clients(Client* clients, int actual);
static void print_online_players(Client* clients, Client* client, int actual);
static void print_menu(Client* client);
static Client* is_client_connected(Client *clients, int actual, char* username);
static void send_request(Client* sender, Client* receiver);
static Client* get_sender_from_receiver(Request* requests, int actual, Client* receiver);
static int get_request_index(Request *requests, int actual, Client* sender);
static void remove_request(Request *requests, int to_remove, int *actual);
static void print_game(Game* game, Client* client, int in_choice);
static void print_board(Game* game, Client* client, int in_choice);
static Client* get_client_from_username(Client* clients, int actual, char* username);
static void print_game_end(Game* game, int status, Client* client_to_send);
static Client* get_receiver_from_sender(Request* requests, int actual, Client* sender);
static void remove_game(Game* games, Game* game, int* actualGame);
static void intToStr(int value, char* buffer, size_t size);
static void print_player_archives(Client* client);
static void print_current_games(Game* games, int actualGame, Client* client);

#endif /* guard */
