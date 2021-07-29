#include "net.h"
#include "log.h"
#include "types.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#define MAX_CLIENTS 3
#define PORT 9999

IPaddress ip;
TCPsocket my_socket;
int num_clients = 0;
TCPsocket clients[MAX_CLIENTS];

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
    
    // resolve as host
    ResolveHost(&ip, NULL);
    
    // open my socket
    my_socket = SDLNet_TCP_Open(&ip);
    if ( !my_socket ) {
        LOG("Could not open TCP socket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    puts("waiting for client connections...");
    int connected_clients = 0;
    
    const u8 * keys = SDL_GetKeyboardState(NULL);
    
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

    ResolveHost(&ip, host_name);
    my_socket = SDLNet_TCP_Open(&ip);
    if ( !my_socket ) {
        LOG("Could not open TCP socket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
}

void NetInit(int argc, char ** argv)
{
    if ( argc == 3 ) {
        if ( strcmp(argv[1], "-host") == 0 ) {
            num_clients = atoi(argv[2]) - 1;
            printf("num clients: %d\n", num_clients);
            if ( num_clients < 1 || num_clients > 3 ) {
                LOG("aborting network game, "
                    "please specific number of players: 2-4\n");
                return;
            }
            InitServer();
        } else if ( strcmp(argv[1], "-connect") == 0 ) {
            InitClients(argv[2]);
        }
    }
}
