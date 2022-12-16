#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "request.h"
#include "../game_server.h"


//--------------------------------------------------------------
//                  UDP Module                                  
//--------------------------------------------------------------


int start_request_handler(socket_ds* sockets_ds) {


}


int play_request_handler(socket_ds* sockets_ds) {


}


int guess_request_handler(socket_ds* sockets_ds) {


}


int quit_request_handler(socket_ds* socket_ds) {


}


// handle requests to separate handlers
void udp_requests_handler(socket_ds* socket_ds) {

    // having the udp socket completely set up
    // we can now process requests from clients
    while(true) {






    }

}

// set up a UDP SOCKET 
void udp_setup(socket_ds* sockets_ds, input_args args) {

    int ret;

    if((sockets_ds->fd_udp = socket(AF_INET, SOCK_DGRAM, AUTO_PROTOCOL)) == ERROR) {
    
        fprintf(stderr, ERROR_FD_UDP);
        exit(EXIT_FAILURE);

    } 

    memset(&sockets_ds->addrinfo_udp, 0, sizeof(sockets_ds->addrinfo_udp));
    sockets_ds->addrinfo_udp.ai_family = AF_INET; //IPv4
    sockets_ds->addrinfo_udp.ai_socktype = SOCK_DGRAM;  //UDP socket
    sockets_ds->addrinfo_udp.ai_flags = AI_PASSIVE; //

    if((ret = getaddrinfo(NULL,args.port,&sockets_ds->addrinfo_udp, &sockets_ds->addrinfo_udp_ptr) != SUCCESS)) {
        fprintf(stderr, ERROR_ADDR_UDP);
		exit(EXIT_FAILURE);

    }

    if(bind(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr->ai_addr,sockets_ds->addrinfo_udp_ptr->ai_addrlen) == ERROR) {
        fprintf(stderr, ERROR_BIND_UDP);
		exit(EXIT_FAILURE);

    }


}


//--------------------------------------------------------------
//                  TCP Module                                  
//--------------------------------------------------------------


int send_scoreboard_request(socket_ds* sockets_ds) {


}


int send_hint_request(socket_ds* sockets_ds) {


}


int send_state_request(socket_ds* sockets_ds) {


}


// handle requests to different functions
void udp_requests_handler(socket_ds* socket_ds) {

    // having the udp socket completely set up
    // we can now process requests from clients
    while(true) {






    }

}

void tcp_setup(socket_ds* sockets_ds, input_args args) {


}
