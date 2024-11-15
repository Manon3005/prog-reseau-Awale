#ifndef REQUEST_H
#define REQUEST_H

typedef struct Client;

typedef struct
{
   Client* sender;
   Client* receiver;
} Request;

#endif /* guard */
