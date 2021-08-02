#ifndef net_h
#define net_h

#include "input.h"

#define SERVER_ID -1

typedef s8 client_id_t;

void NetInit(int argc, char ** argv);
void NetQuit(void);
void ClientSendInputState(InputState input_state);
InputState HostReceiveInputState(client_id_t id);

extern bool is_network_game;
extern client_id_t num_clients;
extern client_id_t my_id;

#endif /* net_h */
