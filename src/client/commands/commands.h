/*
 * File: commands.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for commands.c
 * 
 * 
*/

#ifndef COMMANDS_H
#define COMMANDS_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../player.h"



// Structure that stores information related to SOCKET communication 
// both DATAGRAM AND STREAM SOCKETS
typedef struct {

    int fd_udp;
    int fd_tcp;
    struct addrinfo addrinfo_udp, *addrinfo_udp_ptr;
    struct addrinfo addrinfo_tcp, *addrinfo_tcp_ptr;
} socket_ds; // socket DATAGRAM STREAM




// UDP Function Prototypes 

/*Sending messages: */
int send_start_request(socket_ds*, game_status*);
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

#define ERROR_SEND_UDP "\n"\
						"UDP: An error has occurred while trying to send data over SOCK_GRAM \n"\




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
#define MESSAGE_SIZE 32
#define START_REQUEST_SIZE 11
#define START_RESPONSE_SIZE 12 + 1
#define AUTO_PROTOCOL 0
enum status_code {OK, WIN, DUP, NOK, OVR, INV, ERR};

#endif /* COMMANDS_H */
