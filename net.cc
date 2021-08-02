#include "net.h"
#include "log.h"
#include "types.h"
#include "defines.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#define MAX_CLIENTS (MAX_PLAYERS - 1)
#define PORT 9999

bool is_network_game = false;

IPaddress ip;
TCPsocket my_socket;
TCPsocket clients[MAX_CLIENTS];
client_id_t num_clients = 0;
client_id_t my_id; // if client, index into server's clients[]

void NetQuit()
{
    SDLNet_TCP_Close(my_socket);
    for ( int i = 0; i < num_clients; i++ ) {
        SDLNet_TCP_Close(clients[i]);
    }
}


void ResolveHost(IPaddress *address, const char *host)
{
    if ( SDLNet_ResolveHost(address, host, PORT) == -1 ) {
        LOG("Could not resolve host %s: %s\n", host, SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
}


void InitServer()
{
    printf("Starting a %d player network game as host\n", num_clients + 1);
    
    is_network_game = true;
    my_id = SERVER_ID;
    
    // resolve as host
    ResolveHost(&ip, NULL);
    
    // open my socket
    my_socket = SDLNet_TCP_Open(&ip);
    if ( !my_socket ) {
        LOG("Could not open TCP socket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    
    const u8 * keys = SDL_GetKeyboardState(NULL);

    puts("waiting for client connections...");
    u8 connected_clients = 0;
    while ( connected_clients < num_clients ) {
        clients[connected_clients] = SDLNet_TCP_Accept(my_socket);
        
        if ( clients[connected_clients] ) {
            IPaddress * remote_ip = SDLNet_TCP_GetPeerAddress(clients[connected_clients]);
            u32 address = SDL_SwapBE32(remote_ip->host);
            printf("client connected from %d.%d.%d.%d:%d\n",
                   address >> 24,
                   (address >> 16) & 0xFF,
                   (address >> 8) & 0xFF,
                   address & 0xFF,
                   remote_ip->port);

            u8 client_id = connected_clients;
            printf("sending id %d\n", client_id); // TEMP
            SDLNet_TCP_Send(clients[connected_clients], &client_id, sizeof(client_id));
            
            connected_clients++;
            continue;
        }
        
        if ( keys[SDL_SCANCODE_ESCAPE] ) {
            break;
        }
        
        SDL_Delay(100);
    }
}

void InitClients(char * host_name)
{
    printf("Joing a network game at %s\n", host_name);

    is_network_game = true;
    
    ResolveHost(&ip, host_name);
    my_socket = SDLNet_TCP_Open(&ip);
    if ( !my_socket ) {
        LOG("Could not open TCP socket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    
    SDLNet_TCP_Recv(my_socket, &my_id, sizeof(my_id));
    printf("got id of %d from server\n", my_id);
}


void NetInit(int argc, char ** argv)
{
    if ( argc == 3 ) {
        if ( strcmp(argv[1], "-host") == 0 ) {
            int num_players = atoi(argv[2]);
            if ( num_players < 2 || num_players > MAX_PLAYERS ) {
                LOG("error: please specify 2-%d players\n", MAX_PLAYERS);
                return;
            }

            num_clients = num_players - 1;
            InitServer();
        } else if ( strcmp(argv[1], "-connect") == 0 ) {
            InitClients(argv[2]);
        }
    }
}


void ClientSendInputState(InputState input_state)
{
    int size = (int)sizeof(input_state);
    
    if ( SDLNet_TCP_Send(my_socket, &input_state, size) < size ) {
        fprintf(stderr, "failed to send input state\n");
    }
}



InputState HostReceiveInputState(client_id_t id)
{
    InputState state;
    
    SDLNet_TCP_Recv(clients[id], &state, sizeof(state));
    return state;
}
