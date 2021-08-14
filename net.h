#ifndef net_h
#define net_h

#include "input.h"
#include "defines.h"

#include <SDL2/SDL_net.h>

#define MAX_CLIENTS (MAX_PLAYERS - 1)
#define SERVER_ID -1

typedef s8 client_id_t;

void InitNet(void);
void NetQuit(void);
void ClientSendInputState(InputState input_state);
InputState HostReceiveInputState(client_id_t id);
void NetSendToAllClients(void * data, int len);
void NetSendToClient(client_id_t id, void * data, int len);
void NetSync(void);

extern bool is_network_game;
extern client_id_t num_clients;
extern client_id_t my_id;
extern TCPsocket clients[MAX_CLIENTS];
extern TCPsocket my_socket;

#endif /* net_h */
