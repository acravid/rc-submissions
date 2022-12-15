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
#include <ctype.h>

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
int send_play_request(socket_ds*, game_status*);
int send_guess_request(socket_ds*, game_status*);
int send_quit_request(socket_ds*, game_status*);


void udp_setup(socket_ds*,optional_args);

//  UDP Error messages

#define ERROR_FD_UDP  "\n"\
					 "An error has occurred\n"\
					 "Failed to create an endpoint for communication\n"\
					 "UDP: The request (socket) was not satisfied\n"



#define ERROR_ADDR_UDP "\n"\
						   "An error has occurred\n"\
						   "UDP: The request (getaddrinfo) was not satisfied\n"

#define ERROR_SEND_UDP "\n"\
							"UDP: An error has occurred while trying to exchange data over SOCK_GRAM \n"\


#define ERROR_RECV_UDP "\n"\
					   "UDP: An error has occurred while trying to receive data over SOCK_GRAM\n"


// TCP Function Prototypes 

int send_scoreboard_request(socket_ds*, optional_args, game_status*);
int send_hint_request(socket_ds*, optional_args, game_status*);
void send_state_message();

void tcp_setup(socket_ds*,optional_args);


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
					  
#define ERROR_SEND_TCP "\n"\
						"TCP: An error has occurred while exchanging data \n"\


// Macros
#define PADDING_NULL_TERMINATOR 1
#define MAX_WORD_LENGTH 30

#define ERROR -1
#define MESSAGE_SIZE 32
#define MAX_FILE_INFO_SIZE MAX_FILENAME + MAX_FILE_SIZE_DIGITS + 5
#define START_REQUEST_SIZE 11
#define START_RESPONSE_SIZE 12 + 1
#define PLAY_REQUEST_SIZE 15
#define PLAY_RESPONSE_SIZE 73
#define GUESS_REQUEST_SIZE 46
#define GUESS_RESPONSE_SIZE 10
#define SCOREBOARD_REQUEST_SIZE 4
#define HINT_REQUEST_SIZE 11
#define QUIT_REQUEST_SIZE 11
#define QUIT_RESPONSE_SIZE 4
#define AUTO_PROTOCOL 0
enum status_code {OK, WIN, DUP, NOK, OVR, INV, ERR};

#endif /* COMMANDS_H */
