#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"



// 
//  UDP Module
// 


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
		fprint(stderr,ERROR_TCP_CONNECT);
		exit(EXIT_FAILURE);
	}

}