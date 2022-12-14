#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
		game_stats->running = YES;
		game_stats->trial = 1;
		game_stats->word = (char*) malloc(sizeof(char) * game_stats->letters);
		for (int i = 0; i <= game_stats->letters - 1; i++) {
			game_stats->word[i] = '_';
		}
	}
	else {
		game_stats->running = MAYBE;
		printf("Error. Player ID is invalid or has a game already running on the server.\n");
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
	strcpy(game_stats->player_id, player_id);
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


int process_play_response(char* response, ssize_t ret_recv_udp_response, game_status* game_stats) {
	// turn response into str
	if(ret_recv_udp_response < PLAY_RESPONSE_SIZE) {
		response[ret_recv_udp_response] = '\0';
	}
	else {
		response[PLAY_RESPONSE_SIZE - 1] = '\0';
	}
	printf("formatted response:\n");
	printf("%s\n",response);
	// process response
	if (strcmp(strtok(response, " "), "RLG") != EQUAL) {
		game_stats->last_play = ERR;
		// TO DO: mensagem de erro
		printf("%s", "ERRO\n");
		return ERROR;
	}
	char* code = strtok(NULL, " ");
	if (strcmp(code, "OK") == EQUAL && atoi(strtok(NULL, " ")) == game_stats->trial) {
		game_stats->last_play = OK;
		int n = atoi(strtok(NULL, " "));
		for (; n > 0; n--) {
			game_stats->word[atoi(strtok(NULL, " ")) - 1] = game_stats->last_letter;
		}
		game_stats->trial += 1;
	}
	else if (strcmp(code, "WIN") == EQUAL) {
		game_stats->last_play = WIN;
		for (int i = 0; i < game_stats->letters; i++) {
			if (game_stats->word[i] == '_')
				game_stats->word[i] = game_stats->last_letter;
		}
	}
	else if (strcmp(code, "DUP") == EQUAL) {
		game_stats->last_play = DUP;
	}
	else if (strcmp(code, "NOK") == EQUAL) {
		game_stats->trial += 1;
		game_stats->last_play = NOK;
	}
	else if (strcmp(code, "OVR") == EQUAL) {
		game_stats->last_play = OVR;
	}
	else if (strcmp(code, "INV") == EQUAL) {
		game_stats->last_play = INV;
		printf("%s", "ERRO\n");
		return ERROR;
	}
	else {
		// TO DO: mensagem de erro
		printf("%s", "ERRO\n");
		return ERROR;
	}
	
	return SUCCESS;
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
int send_play_request(socket_ds* sockets_ds, game_status* game_stats) {

	char letter[MAX_STRING];
	char request[PLAY_REQUEST_SIZE];
	char response[PLAY_RESPONSE_SIZE];
	int ret;
	ssize_t ret_send_udp_request, ret_recv_udp_response;
	socklen_t addrlen;
	struct sockaddr_in addr;
	addrlen = sizeof(addr);
	
	// prepare request
	if(game_stats->running == NO) {
		printf("No game running.\n");
		return ERROR;
	}
	
	get_word(letter);
	game_stats->last_letter = toupper(letter[0]);
	sprintf(request, "PLG %s %s %d\n", game_stats->player_id, letter, game_stats->trial);
	
	printf("%s",request);
	// send request over to the server
	int request_size = PLAY_REQUEST_SIZE;
	if (game_stats->trial > 9)
		request_size++;	
	ret_send_udp_request = sendto(sockets_ds->fd_udp, request, request_size, 0, sockets_ds->addrinfo_udp_ptr->ai_addr, sockets_ds->addrinfo_udp_ptr->ai_addrlen);
		
	if(ret_send_udp_request == ERROR) {
		printf("%s", ERROR_SEND_UDP);
		return ERROR;
	}

	// receive the response from the previous request
	ret_recv_udp_response = recvfrom(sockets_ds->fd_udp, response, PLAY_RESPONSE_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(ret_recv_udp_response == ERROR) {
		printf("%s", ERROR_SEND_UDP);
		return ERROR;
	}

	return process_play_response(response, ret_recv_udp_response, game_stats);

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
int process_guess_response(char* response, ssize_t ret_recv_udp_response, game_status* game_stats) {
	
	// turn response into str
	if(ret_recv_udp_response < GUESS_RESPONSE_SIZE) {
		
		response[ret_recv_udp_response] = '\0';
	}
	else {
		response[GUESS_RESPONSE_SIZE - 1] = '\0';
		
	}

	printf("Received response:\n");	
	printf("%s\n",response);

	// process response
	if (strcmp(strtok(response, " "), "RWG") != EQUAL) {
		game_stats->last_play = ERR;
		// TO DO: mensagem de erro
		printf("%s", "ERRO\n");
		return ERROR;
	}

	char* status_code = strtok(NULL, " ");

	if (strcmp(status_code, "OK") == EQUAL && atoi(strtok(NULL, " ")) == game_stats->trial) {
		game_stats->last_play = OK;
		int n = atoi(strtok(NULL, " "));
		for (; n > 0; n--) {
			game_stats->word[atoi(strtok(NULL, " ")) - 1] = game_stats->last_letter;
		}
		game_stats->trial += 1;
	}
	else if (strcmp(status_code, "WIN") == EQUAL) {
		game_stats->last_play = WIN;
		for (int i = 0; i < game_stats->letters; i++) {
			if (game_stats->word[i] == '_')
				game_stats->word[i] = game_stats->last_letter;
		}
	}
	else if (strcmp(status_code, "NOK") == EQUAL) {
		game_stats->trial += 1;
		game_stats->last_play = NOK;
	}
	else if (strcmp(status_code, "OVR") == EQUAL) {
		game_stats->last_play = OVR;
	}
	else if (strcmp(status_code, "INV") == EQUAL) {
		game_stats->last_play = INV;
		printf("%s", "ERRO\n");
		return ERROR;
	}
	else {
		// TO DO: error message
		// received status_code does not match known 
		printf("%s", "ERRO\n");
		return ERROR;
	}
	
	return SUCCESS;
	
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
int send_guess_request(socket_ds* sockets_ds, game_status* game_stats) {


	char word[MAX_WORD_LENGTH + PADDING_NULL_TERMINATOR];
	char request[GUESS_REQUEST_SIZE];
	char response[GUESS_RESPONSE_SIZE];
	int ret;
	ssize_t ret_send_udp_request, ret_recv_udp_response;
	socklen_t addrlen;
	struct sockaddr_in addr;
	addrlen = sizeof(addr);

	// prepare request
	get_word(word);
	memset(request,'\0',sizeof(request));
	sprintf(request, "PWG %s %s %d\n",game_stats->player_id,word,game_stats->trial);

	// send request over to the server
	ret_send_udp_request = sendto(sockets_ds->fd_udp, request, strlen(request) , 0, sockets_ds->addrinfo_udp_ptr->ai_addr, sockets_ds->addrinfo_udp_ptr->ai_addrlen);

	if(ret_send_udp_request == ERROR) {
		printf("%s", ERROR_SEND_UDP);
		return ERROR;
	}

	// receive the response from the previous request
	ret_recv_udp_response = recvfrom(sockets_ds->fd_udp, response, GUESS_RESPONSE_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(ret_recv_udp_response == ERROR) {
		printf("%s", ERROR_RECV_UDP);
		return ERROR;
	}
	
	return process_guess_response(response, ret_recv_udp_response, game_stats);

}








void send_scoreboard_message() {}
void send_hint_message() {}
void send_state_message() {}

int process_quit_response(char* response, ssize_t ret_recv_udp_response, game_status* game_stats) {
	// turn response into str
	if(ret_recv_udp_response < START_RESPONSE_SIZE) {
		response[ret_recv_udp_response] = '\0';
	}
	else {
		response[START_RESPONSE_SIZE - 1] = '\0';
	}
	
	game_stats->running = NO;
	// process response
	if (strcmp(strtok(response, " "), "RQT") == EQUAL)
		return SUCCESS;
	
	printf("Invalid player ID.\n");
	return ERROR;
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
int send_quit_request(socket_ds* sockets_ds, game_status* game_stats) {

	char request[START_REQUEST_SIZE];
	char response[START_RESPONSE_SIZE];
	int ret;
	ssize_t ret_send_udp_request, ret_recv_udp_response;
	socklen_t addrlen;
	struct sockaddr_in addr;
	addrlen = sizeof(addr);
	
	// prepare request
	if(game_stats->running == NO) {
		printf("No game running.\n");
		return ERROR;
	}
	sprintf(request, "QUT %s\n", game_stats->player_id);


	// send request over to the server
	ret_send_udp_request = sendto(sockets_ds->fd_udp, request, QUIT_REQUEST_SIZE, 0, sockets_ds->addrinfo_udp_ptr->ai_addr, sockets_ds->addrinfo_udp_ptr->ai_addrlen);
	if(ret_send_udp_request == ERROR) {
		printf("%s", ERROR_SEND_UDP);
		return ERROR;
	}

	// receive the response from the previous request
	ret_recv_udp_response = recvfrom(sockets_ds->fd_udp, response, QUIT_RESPONSE_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(ret_recv_udp_response == ERROR) {
		printf("%s", ERROR_SEND_UDP);
		return ERROR;
	}
	
	return process_quit_response(response, ret_recv_udp_response, game_stats);
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
