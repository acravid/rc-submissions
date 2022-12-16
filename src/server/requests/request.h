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
int start_request_handler(socket_ds*);
int play_request_handler(socket_ds*);
int guess_request_handler(socket_ds*);
int quit_request_handler(socket_ds*);

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
int scoreboard_request_handler(socket_ds*);
int hint_request_handler(socket_ds*);
int state_request_handler(socket_ds*);

void tcp_setup(socket_ds*, input_args);


//--------------------------------------------------------------
//                              MACROS
//--------------------------------------------------------------

#define AUTO_PROTOCOL 0
#define ERROR -1
#define SUCCESS 0
#endif /* REQUEST_H */
