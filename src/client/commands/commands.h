/*
 * File: commands.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for commands.c
 * 
 * 
*/

#ifndef COMMANDS_H
#define COMMANDS_H

#include "../player.h"
#include "../../common/common.h"



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
void send_start_message(socket_ds *socket_ds_ptr);
void send_play_message(socket_ds *socket_ds_ptr);
void send_guess_message(socket_ds *socket_ds_ptr);
void send_quit_message(socket_ds *socket_ds_ptr);
void send_exit_message(socket_ds *socket_ds_ptr);


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
#define SNG_REQUEST_SIZE 9
#define SNG_RESPONSE_SIZE 9
#define SNG_RESPONSE "New game started (max %c errors): %s"
#define REQUEST_SIZE_SNG 9
#define RESPONSE_SIZE_SNG 13
#define 



#endif /* COMMANDS_H */
