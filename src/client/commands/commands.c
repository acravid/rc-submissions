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
void send_start_message(socket_ds *socket_ds_ptr) {

	char *player_id;
	ssize_t ret_send_udp_request, ret_recv_udp_response;
	int ret;
	
	byte_buffer request, response;
	buffer_init(request,REQUEST_SIZE_SNG,char);
	buffer_init(response,RESPONSE_SIZE_SNG,char)

	player_id = strtok(NULL," ");

	
	sprintf(request.info,"SNG %s\n",player_id);
	

	// send request over to the server 
	ret_send_udp_request = send_udp_request(socket_ds_ptr->fd_udp,request.info,request.info,socket_ds_ptr->addrinfo_udp);

	if(ret_send_udp_request == ERROR) {
		fprintf("%s",ERROR_SEND_UDP);
		exit(EXIT_FAILURE);
	}

	

	// receive the response from the previous request
	ret_recv_udp_response = rcv_udp_response(socket_ds_ptr->fd_udp,response.info, response.size, socket_ds_ptr->addrinfo_udp);
	if(ret_recv_udp_response == ERROR) {
		fprintf("%s",ERROR_SEND_UDP);
		exit(EXIT_FAILURE);

	}
	if(ret_recv_udp_response < response.size) {
		response.info[ret_recv_udp_response] = '\0';
	}

	// by now we've already received the response 
	// in the following formart 
	// e.g RSG OK 10 8


	// process the response 
	// and output correctly




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
void send_play_message(socket_ds *socket_ds_ptr) {








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
void send_guess_message(socket_ds *socket_ds_ptr) {






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
void send_quit_message(socket_ds *socket_ds_ptr) {







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