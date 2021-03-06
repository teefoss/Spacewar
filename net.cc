#include "net.h"
#include "log.h"
#include "types.h"
#include "app.h"

#include <SDL2/SDL.h>

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
    
    for ( int i = 0; i < num_clients; i++ ) {
        printf("sending client %d num_clients (%d)\n", i, num_clients); // TEMP
        SDLNet_TCP_Send(clients[i], &num_clients, sizeof(num_clients));
    }
}

void InitClients(const char * host_name)
{
    printf("Joining a network game at %s\n", host_name);

    is_network_game = true;
    
    ResolveHost(&ip, host_name);
    my_socket = SDLNet_TCP_Open(&ip);
    if ( !my_socket ) {
        LOG("Could not open TCP socket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    
    SDLNet_TCP_Recv(my_socket, &my_id, sizeof(my_id));
    printf("got id of %d from server\n", my_id);
    
    SDLNet_TCP_Recv(my_socket, &num_clients, sizeof(num_clients));
    printf("server sent num_clients: %d\n", num_clients);
}


void InitNet()
{
    Args args = App::Shared()->GetArgs();
    
    if ( args.count == 3 ) {
        if ( strcmp(args[1], "-host") == 0 ) {
            int num_players = atoi(args[2]);
            if ( num_players < 2 || num_players > MAX_PLAYERS ) {
                LOG("error: please specify 2-%d players\n", MAX_PLAYERS);
                return;
            }

            num_clients = num_players - 1;
            InitServer();
        } else if ( strcmp(args[1], "-connect") == 0 ) {
            InitClients(args[2]);
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


void NetSend(void * data, int len)
{
    SDLNet_TCP_Send(my_socket, data, len);
}


void NetReceive(void * data, int len)
{
    SDLNet_TCP_Recv(my_socket, data, len);
}


void NetSendToAllClients(void * data, int len)
{
    for ( client_id_t id = 0; id < num_clients; id++ ) {
        NetSendToClient(id, data, len);
    }
}


void NetSendToClient(client_id_t id, void * data, int len)
{
    SDLNet_TCP_Send(clients[id], data, len);
}


void NetSync()
{
    u8 sync = 1;
    
    if ( my_id == SERVER_ID ) {
        for ( int i = 0; i < num_clients; i++ ) {
            SDLNet_TCP_Recv(my_socket, &sync, sizeof(sync));
        }
    } else {
        SDLNet_TCP_Send(my_socket, &sync, sizeof(sync));
    }
}
