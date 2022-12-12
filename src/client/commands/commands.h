/*
 * File: commands.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for commands.c
 * 
 * 
*/

#ifndef COMMANDS_H
#define COMMANDS_H

#include "player.h"
#include "../common/common.h"



// Structure that stores information related to SOCKET communication 
// both DATAGRAM AND STREAM SOCKETS
typedef struct {

    int fd_udp;
    int fd_tcp;
    struct addrinfo *addrinfo_udp;
    struct addrinfo *addrinfo_tcp;
} socket_ds; // socket DATAGRAM STREAM




// UDP Function Prototypes 

/*Sending messages: */
void send_start_message();
void send_play_message();
void send_guess_message();
void send_quit_message();

void udp_setup(socket_ds *sockets_ds,optional_args opt_args);

//  UDP Error messages

#define ERROR_FD_UDP  "\n"\
					 "An error has occurred\n"\
					 "Failed to create an endpoint for communication\n"\
					 "UDP: The request (socket) was not satisfied\n"



#define ERROR_ADDR_UDP "\n"\
						   "An error has occurred\n"\
						   "UDP: The request (getaddrinfo) was not satisfied\n"




// TCP Function Prototypes 

void send_scoreboard_message();
void send_hint_message();
void send_state_message();

void tcp_setup(socket_ds *sockets_ds,optional_args opt_args);


//  TCP Error messages

#define ERROR_FD_TCP  "\n"\
					 "An error has occurred\n"\
					 "Failed to create an endpoint for communication\n"\
					 "TCP: The request (socket) was not satisfied\n"

#define ERROR_TCP_CONNECT "\n"\
					 "An error has occurred\n"\
					 "TCP: The binding or connection was not successful\n"


#define ERROR_ADDR_TCP "\n"\
					  "An error has occurred\n"\
					  "TCP: The request (getaddrinfo) was not successful\n"


// Macros

#define ERROR -1
            

#endif /* COMMANDS_H */
