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

// NOTE: move common macros  and functions to a shared file (client,server)

// Structure that stores information related to SOCKET communication 
// both DATAGRAM AND STREAM SOCKETS
typedef struct {

    int fd_udp;
    int fd_tcp;
    struct addrinfo addrinfo_udp, *addrinfo_udp_ptr;
    struct addrinfo addrinfo_tcp, *addrinfo_tcp_ptr;
	
} socket_ds; // socket DATAGRAM STREAM


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
#define SERVER_UDP_MAX_REPLY_SIZE 76
#define GAME_PLAY_CODE_SIZE 3
#define CLIENT_TCP_MAX_REQUEST_SIZE 11
#define SERVER_TCP_MAX_REPLY_SIZE 100000 // FIX ME LATER

#define PLAYERID_MIN 90000
#define PLAYERID_MAX 110000
#define MAX_QUEUED_REQUESTS 10
#define FORK_CHILD 0




// known udp requests game_code
#define START_CODE "SNG"
#define PLAY_CODE "PLG"
#define GUESS_CODE "PWG"
#define QUIT_CODE "RQT"
#define EXIT_CODE "RQT"
#define DEBUG_CODE "REV"

#define START_REPLY_CODE "RSG"
#define PLAY_REPLY_CODE "RLG"

// known tcp requests 
#define SCOREBOARD_CODE "GSB"
#define HINT_CODE "GHL"
#define STATUS_CODE "STA"


#define NOK_REPLY_CODE "NOK"
#define DUP_REPLY_CODE "DUP"
#define INV_REPLY_CODE "INV"
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
