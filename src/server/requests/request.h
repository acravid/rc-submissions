/*
 * File: requests.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for requests.c
 * 
 * 
*/

#ifndef REQUESTS_H
#define REQUESTS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h> 
#include "../GS.h"

#define MAX_WORD_SIZE 31
#define SIZE 1000

// NOTE: move common macros  and functions to a shared file (client,server)

// Structure that stores information related to SOCKET communication 
// both DATAGRAM AND STREAM SOCKETS
typedef struct {

    int fd_udp;
    int fd_tcp;
    struct addrinfo addrinfo_udp, *addrinfo_udp_ptr;
    struct addrinfo addrinfo_tcp, *addrinfo_tcp_ptr;
	
} socket_ds; // socket DATAGRAM STREAM

typedef struct {
	char word[MAX_WORD_SIZE];
	char hint[1000];
	int n_letters;
	int n_errors;
	char played_letters[27];
	int trial;
} player_info;

void init_player_info(input_args);

//--------------------------------------------------------------
//                  UDP Function Prototypes                                  
//--------------------------------------------------------------


// handle udp requests from client
void start_request_handler(char*,size_t,char*);
void play_request_handler(char*,size_t,char*);
void guess_request_handler(char*,size_t,char*);
void quit_request_handler(char*,size_t,char*);
void exit_request_handler(char*,size_t,char*);
void debug_request_handler(char*,size_t,char*);

void udp_setup(socket_ds*, input_args);
void udp_request_handler(socket_ds*);

//  UDP Error messages

#define ERROR_FD_UDP  "\n"\
					 "An error has occurred\n"\
					 "Failed to create an endpoint for communication\n"\
					 "UDP: The request (socket) was not satisfied\n"

#define ERROR_ADDR_UDP "\n"\
						   "An error has occurred\n"\
						   "UDP: The request (getaddrinfo) was not satisfied\n"

// TODO:
#define ERROR_BIND_UDP "\n"\

//--------------------------------------------------------------
//                  TCP Function Prototypes                                  
//--------------------------------------------------------------


// handle tcp requests from client

void scoreboard_request_handler(char*,size_t,char*);
void hint_request_handler(char*,size_t,char*);
void state_request_handler(char*,size_t,char*);

void tcp_setup(socket_ds*, input_args);
void tcp_request_handler(socket_ds*);



/*TCP error messages: */
#define ERROR_FD_TCP  "\n"\
					 "An error has occurred\n"\
					 "Failed to create an endpoint for communication\n"\
					 "TCP: socket() was not satisfied\n"

#define ERROR_ADDR_TCP "\n"\
					  "An error has occurred\n"\
					  "TCP:  getaddrinfo() was not successful\n"

// TODO:
#define ERROR_BIND_TCP "\n"\
					   "An error has occurred\n"\
					   "TCP: bind() was not successful\n"

#define ERROR_LISTEN "\n"\
					 "ERROR "


//--------------------------------------------------------------
//                              MACROS
//--------------------------------------------------------------

#define AUTO_PROTOCOL 0
#define ERROR -1
#define SUCCESS 0
#define CLIENT_UDP_MAX_REQUEST_SIZE  46 // guess request
#define MAX_GUESS_REPLY_SIZE 11
#define MAX_PLAY_REPLY_SIZE 76
#define CODE_SIZE 3
#define CLIENT_TCP_MAX_REQUEST_SIZE 11
#define SERVER_TCP_MAX_REPLY_SIZE 100000 // FIX ME LATER

#define PLAYERID_MIN 90000
#define PLAYERID_MAX 110000
#define ASCII_A 65
#define ASCII_z 122
#define PLAYERID_SIZE 6

#define MAX_QUEUED_REQUESTS 10
#define FORK_CHILD 0




// known udp requests game_code
#define START_CODE "SNG"
#define PLAY_CODE "PLG"
#define GUESS_CODE "PWG"
#define QUIT_CODE "QUT"
#define DEBUG_CODE "REV"

#define START_REPLY_CODE "RSG"
#define PLAY_REPLY_CODE "RLG"
#define GUESS_REPLY_CODE "RWG"
#define QUIT_REPLY_CODE "RQT"

// known tcp requests 
#define SCOREBOARD_CODE "GSB"
#define HINT_CODE "GHL"
#define STATE_CODE "STA"

#define SCOREBOARD_REPLY_CODE "RSB"
#define HINT_REPLY_CODE "RHL"
#define STATE_REPLY_CODE "RST"


#define OK_REPLY_CODE "OK"
#define NOK_REPLY_CODE "NOK"
#define WIN_REPLY_CODE "WIN"
#define OVR_REPLY_CODE "OVR"
#define DUP_REPLY_CODE "DUP"
#define INV_REPLY_CODE "INV"
#define EMPTY_REPLY_CODE "EMPTY"
#define ERROR_REPLY_CODE "ERR"



// generic ERROR messages
#define ERROR_RECV_FROM "\n"\
						"recvfrom(): an error has occurred, failed to receive message from socket\n"

#define ERROR_SENDO_TO "\n"\
						"sendto(): an error has occurred, failed to send message on socket\n"
				

#define ERROR_ACCEPT "\n"\
					 "accept(): an error has occurred, failed to accept connection on socket\n"
#define ERROR_CLOSE "\n"\
					"close(): an error has occurred, failed to close the file\n"

#define ERROR_READ "\n"\
				   "read(): an erros has occurred, failed to read ... bytes\n"

/* Clean up socket resources: */
void cleanup_connection(int,struct addrinfo*);



#endif /* REQUEST_H */
