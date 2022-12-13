#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

// 
//  UDP Module
// 


int process_start_response(char* response, ssize_t ret_recv_udp_response, game_status* game_stats) {
	// turn response into str
	if(ret_recv_udp_response < START_RESPONSE_SIZE) {
		response[ret_recv_udp_response] = '\0';
	}
	else {
		response[START_RESPONSE_SIZE - 1] = '\0';
	}
	
	// process response
	if (strcmp(strtok(response, " "), "RSG") == EQUAL && strcmp(strtok(NULL, " "), "OK") == EQUAL) {
		game_stats->letters = atoi(strtok(NULL, " "));
		game_stats->errors = atoi(strtok(NULL, " "));
	}
	else {
		// TO DO: mensagem de erro
		printf("%s", "ERRO\n");
		return ERROR;
	}
	
	return SUCCESS;
}

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
int send_start_request(socket_ds* sockets_ds, game_status* game_stats) {

	char player_id[MAX_STRING];
	char request[START_REQUEST_SIZE];
	char response[START_RESPONSE_SIZE];
	int ret;
	ssize_t ret_send_udp_request, ret_recv_udp_response;
	socklen_t addrlen;
	struct sockaddr_in addr;
	addrlen = sizeof(addr);
	
	// prepare request
	get_word(player_id);
	sprintf(request, "SNG %s\n", player_id);

	// send request over to the server
	ret_send_udp_request = sendto(sockets_ds->fd_udp, request, START_REQUEST_SIZE, 0, sockets_ds->addrinfo_udp_ptr->ai_addr, sockets_ds->addrinfo_udp_ptr->ai_addrlen);
	if(ret_send_udp_request == ERROR) {
		printf("%s", ERROR_SEND_UDP);
		return ERROR;
	}

	// receive the response from the previous request
	ret_recv_udp_response = recvfrom(sockets_ds->fd_udp, response, START_RESPONSE_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(ret_recv_udp_response == ERROR) {
		printf("%s", ERROR_SEND_UDP);
		return ERROR;
	}
	
	return process_start_response(response, ret_recv_udp_response, game_stats);
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

void send_scoreboard_message() {}
void send_hint_message() {}
void send_state_message() {}
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

    sockets_ds->fd_udp = socket(AF_INET, SOCK_DGRAM, AUTO_PROTOCOL);
    if(sockets_ds->fd_udp == ERROR) {
        fprintf(stderr, ERROR_FD_UDP);
        exit(EXIT_FAILURE);
    }

    memset(&sockets_ds->addrinfo_udp, 0, sizeof(sockets_ds->addrinfo_udp));
    // set hints
    sockets_ds->addrinfo_udp.ai_family = AF_INET; //IPv4
    sockets_ds->addrinfo_udp.ai_socktype = SOCK_DGRAM;  //UDP socket
    int ret = getaddrinfo(opt_args.ip, opt_args.port, &sockets_ds->addrinfo_udp, &sockets_ds->addrinfo_udp_ptr);
    if(ret != SUCCESS) {
        // Failed to get an internet address
        freeaddrinfo(sockets_ds->addrinfo_udp_ptr);
		close(sockets_ds->fd_udp);
		fprintf(stderr, ERROR_ADDR_UDP);
		exit(EXIT_FAILURE);
    }

}



// 
//  TCP Module
// 

/*
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

}*/
