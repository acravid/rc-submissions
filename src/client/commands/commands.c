#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"



buffer_typedef(char,byte_buffer);

// 
//  UDP Module
// 



// TODO:
// complete function's brief

//
// Function:
//
//
// Inputs: 
//
//
// Description:
//
//
// 
void send_start_message() {

	char *player_id;
	int ret;
	
	byte_buffer message;
	buffer_init(message,MESSAGE_SIZE,char);

	player_id = strtok(NULL," ");

	
	sprintf(message.info,"SNG %s\n",player_id);

	ret = send_udp_message();

	if(ret == OK) {
		
	}

	








	// return value from the udp call
	ret;






}


//
// Function:
//
//
// Inputs: 
//
//
// Description:
//
//
//
void send_play_message() {








}


//
// Function:
//
//
// Inputs: 
//
//
// Description:
//
//
//
void send_guess_message() {






}


//
// Function:
//
//
// Inputs: 
//
//
// Description:
//
//
//
void send_quit_message() {







}


void udp_setup(socket_ds *sockets_ds, optional_args opt_args) {

    sockets_ds->fd_udp = socket(AF_INET,SOCK_DGRAM,AUTO_PROTOCOL);
	
    if(sockets_ds->fd_udp == ERROR) {
        fprintf(stderr, ERROR_FD_UDP);
        exit(EXIT_FAILURE);
    }

	// set hints args and get the internet address                             IPv4    UDP
    sockets_ds->addrinfo_udp = getaddrinfo_extended(opt_args.ip,opt_args.port,AF_INET,SOCK_DGRAM, AUTO_PROTOCOL);
    
	if(sockets_ds->addrinfo_udp == NULL) {
		// Failed to get an internet address
		close(sockets_ds->fd_udp);
		fprintf(stderr, ERROR_ADDR_UDP);
		exit(EXIT_FAILURE);

	}


}



// 
//  TCP Module
// 


void tcp_setup(socket_ds *sockets_ds, optional_args opt_args) {


	int errno;

    sockets_ds->fd_tcp = socket(AF_INET,SOCK_STREAM,AUTO_PROTOCOL);


	if(sockets_ds->fd_tcp == ERROR) {
		fprintf(stderr,ERROR_FD_TCP);
		exit(EXIT_FAILURE);
	}

	//set hints args and get the internet address

    sockets_ds->addrinfo_tcp = getaddrinfo_extended(opt_args.ip,opt_args.port,AF_INET,SOCK_STREAM,AUTO_PROTOCOL);

	if(sockets_ds->addrinfo_tcp == NULL) {
        // Failed to get an internet address 
		fprintf(stderr,ERROR_ADDR_TCP);
		exit(EXIT_FAILURE);

    }
 
	// if the connection or binding succeeds,zero is returned.
	// On error, -1 is returned, and errno is set to indicate the error.
	// Reference: man pages
	errno = connect(sockets_ds->fd_tcp, sockets_ds->addrinfo_tcp->ai_addr, sockets_ds->addrinfo_tcp->ai_addrlen);
	
    if( errno == ERROR) {
		fprintf(stderr,ERROR_TCP_CONNECT);
		exit(EXIT_FAILURE);
	}

}