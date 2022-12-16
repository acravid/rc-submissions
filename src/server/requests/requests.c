#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "request.h"
#include "../game_server.h"


//--------------------------------------------------------------
//                  UDP Module                                  
//--------------------------------------------------------------


void start_request_handler(char *buffer,size_t len,char *reply_ptr) {


}


void play_request_handler(char *buffer,size_t len,char *reply_ptr) {


}


void guess_request_handler(char *buffer,size_t len,char *reply_ptr) {


}


void quit_request_handler(char *buffer,size_t len,char *reply_ptr) {


}

void exit_request_handler(char *buffer,size_t len,char *reply_ptr) {


}

void debug_request_handler(char *buffer,size_t len,char *reply_ptr) {


}

void udp_select_requests_handler(char *buffer,size_t len,char *reply_ptr) {

    if(strncmp(START_CODE,buffer,GAME_PLAY_CODE_SIZE)) {
        start_request_handler(buffer,len,reply_ptr);
    } 
    else if(strncmp(PLAY_CODE,buffer,GAME_PLAY_CODE_SIZE)) {
        play_request_handler(buffer,len,reply_ptr);
    }
    else if(strncmp(GUESS_CODE,buffer,GAME_PLAY_CODE_SIZE)) {
        guess_request_handler(buffer,len,reply_ptr);
    }
    else if(strncmp(QUIT_CODE,buffer,GAME_PLAY_CODE_SIZE)) {
        quit_request_handler(buffer,len,reply_ptr);
    }
    else if(strncmp(EXIT_CODE,buffer,GAME_PLAY_CODE_SIZE)) {
        exit_request_handler(buffer,len,reply_ptr);
    } 
    else if(strncmp(DEBUG_CODE,buffer,GAME_PLAY_CODE_SIZE)) {
        exit_request_handler(buffer,len,reply_ptr);
    } else {
        strcpy(reply_ptr,UNKNOWN_GAME_PLAY_CODE_REPLY);

    }

}


// handle requests to separate handlers
void udp_requests_handler(socket_ds* sockets_ds) {

    // having the udp socket completely set up
    // we can now process requests from clients
    char buffer[CLIENT_UDP_MAX_REQUEST_SIZE];
    char reply[SERVER_UDP_MAX_REPLY_SIZE];
 
    memset(reply,'\0',sizeof(reply));
    
    struct sockaddr_in addr;
    socklen_t addrlen;
    ssize_t n, nread;

    while(true) {

        addrlen = sizeof(addr);

        // receive client message from socket
        nread = recvfrom(sockets_ds->fd_udp,buffer,CLIENT_UDP_MAX_REQUEST_SIZE,AUTO_PROTOCOL,(struct sockaddr*)&addr,addrlen);
        if(nread == ERROR) {
            cleanup_connection(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr);
            fprint(stderr,ERROR_RECV_FROM);
            exit(EXIT_FAILURE);
        }

        // protocol states that request and reply end with '\n'
        if(buffer[nread - 1] == '\n') {

            // turn request to string 
            buffer[nread - 1] = '\0';
            udp_select_requests_handler(buffer,nread,&buffer);
            
        } else {
            strcpy(&buffer,UNKNOWN_GAME_PLAY_CODE_REPLY);

        }

        // send reply back to client
        n = sendto(sockets_ds->fd_udp,reply,strlen(reply),AUTO_PROTOCOL,(struct sockaddr*)&addr,addrlen);
        if(n == ERROR) {
            cleanup_connection(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr);
            fprint(stderr,ERROR_SENDO_TO_FROM);
            exit(EXIT_FAILURE);
        }

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
void tcp_requests_handler(socket_ds* socket_ds) {

    // having the udp socket completely set up
    // we can now process requests from clients
    while(true) {






    }

}

void tcp_setup(socket_ds* sockets_ds, input_args args) {


}

// frees addrinfo and closes connection (fd)
void cleanup_connection(int fd,struct addrinfo *addr) {

	freeeaddrinfo(addr);
	close(fd);
	
}