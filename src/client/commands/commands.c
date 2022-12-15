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
	if(strcmp(strtok(response, " "), "RSG") == EQUAL && strcmp(strtok(NULL, " "), "OK") == EQUAL) {
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


	// process response
	if(strcmp(strtok(response, " "), "RLG") != EQUAL) {
		game_stats->last_play = ERR;
		// TO DO: mensagem de erro
		printf("%s", "ERRO\n");
		return ERROR;
	}
	char* status_code = strtok(NULL, " ");
	if(strcmp(status_code, "OK") == EQUAL && atoi(strtok(NULL, " ")) == game_stats->trial) {
		game_stats->last_play = OK;
		int n = atoi(strtok(NULL, " "));
		for(; n > 0; n--) {
			game_stats->word[atoi(strtok(NULL, " ")) - 1] = game_stats->last_letter;
		}
		game_stats->trial += 1;
	}
	else if(strcmp(status_code, "WIN") == EQUAL) {
		game_stats->last_play = WIN;
		for(int i = 0; i < game_stats->letters; i++) {
			if(game_stats->word[i] == '_'){
				game_stats->word[i] = game_stats->last_letter;
			}
		}
	}
	else if(strcmp(status_code, "DUP") == EQUAL) {
		game_stats->last_play = DUP;
	}
	else if(strcmp(status_code, "NOK") == EQUAL) {
		game_stats->trial += 1;
		game_stats->last_play = NOK;
	}
	else if(strcmp(status_code, "OVR") == EQUAL) {
		game_stats->last_play = OVR;
	}
	else if(strcmp(status_code, "INV") == EQUAL) {
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
	
	// send request over to the server
	int request_size = PLAY_REQUEST_SIZE;
	if(game_stats->trial > 9)
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

	// process response
	if(strcmp(strtok(response, " "), "RWG") != EQUAL) {
		game_stats->last_play = ERR;
		// TO DO: mensagem de erro
		printf("%s", "ERRO\n");
		return ERROR;
	}

	char* status_code = strtok(NULL, " ");

	if(strcmp(status_code, "OK") == EQUAL && atoi(strtok(NULL, " ")) == game_stats->trial) {
		game_stats->last_play = OK;
		int n = atoi(strtok(NULL, " "));
		for (; n > 0; n--) {
			game_stats->word[atoi(strtok(NULL, " ")) - 1] = game_stats->last_letter;
		}
		game_stats->trial += 1;
	}
	else if(strcmp(status_code, "WIN") == EQUAL) {
		game_stats->last_play = WIN;
	}
	else if(strcmp(status_code, "NOK") == EQUAL) {
		game_stats->trial += 1;
		game_stats->last_play = NOK;
	}
	else if(strcmp(status_code, "OVR") == EQUAL) {
		game_stats->last_play = OVR;
	}
	else if(strcmp(status_code, "INV") == EQUAL) {
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
	upcase_word(word);
	memset(request,'\0',sizeof(request));
	game_stats->guess = word;
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
	if(strcmp(strtok(response, " "), "RQT") == EQUAL)
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
void tcp_setup(socket_ds *sockets_ds, optional_args opt_args) {

    sockets_ds->fd_tcp = socket(AF_INET, SOCK_STREAM, AUTO_PROTOCOL);
	if(sockets_ds->fd_tcp == ERROR) {
		fprintf(stderr,ERROR_FD_TCP);
		exit(EXIT_FAILURE);
	}

	memset(&sockets_ds->addrinfo_tcp, 0, sizeof(sockets_ds->addrinfo_tcp));
    // set hints
    sockets_ds->addrinfo_tcp.ai_family = AF_INET; //IPv4
    sockets_ds->addrinfo_tcp.ai_socktype = SOCK_STREAM;  //TCP socket
    int ret = getaddrinfo(opt_args.ip, opt_args.port, &sockets_ds->addrinfo_tcp, &sockets_ds->addrinfo_tcp_ptr);
	if(ret != SUCCESS) {
        // Failed to get an internet address
        freeaddrinfo(sockets_ds->addrinfo_tcp_ptr);
		close(sockets_ds->fd_tcp);
		fprintf(stderr,ERROR_ADDR_TCP);
		exit(EXIT_FAILURE);

    }
 
	ret = connect(sockets_ds->fd_tcp, sockets_ds->addrinfo_tcp_ptr->ai_addr, sockets_ds->addrinfo_tcp_ptr->ai_addrlen);
    if(ret == ERROR) {
    	freeaddrinfo(sockets_ds->addrinfo_tcp_ptr);
		close(sockets_ds->fd_tcp);
		fprintf(stderr,ERROR_TCP_CONNECT);
		exit(EXIT_FAILURE);
	}

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
int process_scoreboard_response(socket_ds* sockets_ds, game_status* game_stats) {
	
	char filename[MAX_FILENAME + SCOREBOARD_PATHNAME_SIZE];
	int filesize;
	ssize_t n;
	char info[39];
	size_t r_buffer = 0;
	
	// read status
	n = read(sockets_ds->fd_tcp, info, 6);
	if(n != 3 && n != 6) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}
	info[n] = '\0';

	if (strcmp(info, "EMPTY") == EQUAL) {
		printf("Scoreboard is empty.\n");
		return ERROR;
	}
	
	// read filename and size
	n = read(sockets_ds->fd_tcp, info, 39);
	if(n == -1) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}
	
	strcpy(filename, strtok(info, " "));
	filesize = atoi(strtok(NULL, " "));
	char* filedata = (char*) malloc(sizeof(char) * (filesize));
	
	// if last read got part of the file data, copy that part to filedata and move the buffer
	strcpy(filedata, strtok(NULL, " "));
	if(filedata != NULL) r_buffer = strlen(filedata);
	
	// read filedata
	while(r_buffer < filesize) {
		n = read(sockets_ds->fd_tcp, filedata + r_buffer, filesize - r_buffer);
		if (n == ERROR) {
			printf(ERROR_SEND_TCP);
			return ERROR;
		}
		r_buffer += n;
	}

	sprintf(game_stats->scoreboard_filename, "%s%s", SCOREBOARD_PATHNAME, filename);
	FILE* file = fopen(game_stats->scoreboard_filename, "w");
	if(file == NULL) return ERROR;
	
	for(int i = 0; i < filesize; i++) {
	    if(fputc(filedata[i], file) == EOF)
            return ERROR;
    }
    
	free(filedata);
	fclose(file);
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
int send_scoreboard_request(socket_ds* sockets_ds, optional_args opt_args, game_status* game_stats) {
	
	char code[4];
	ssize_t n;

	tcp_setup(sockets_ds, opt_args);

	// send request over to the server
	n = write(sockets_ds->fd_tcp, "GSB\n", SCOREBOARD_REQUEST_SIZE);
	if(n != SCOREBOARD_REQUEST_SIZE) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}
	
	// receive the response from the previous request
	n = read(sockets_ds->fd_tcp, code, 4);
	if(n != 4) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}

	// turn code into a string
	code[3] = '\0';
	if(strcmp(code, "RSB") != EQUAL) {
		printf("ERRO\n");
		return ERROR;
	}

	int res = process_scoreboard_response(sockets_ds, game_stats);
	
	freeaddrinfo(sockets_ds->addrinfo_tcp_ptr);
	close(sockets_ds->fd_tcp);
	
	return res;

}


int process_hint_response(socket_ds* sockets_ds, game_status* game_stats) {

	char file_info[MAX_FILE_INFO_SIZE];
	char status[4];
	char filename[MAX_FILENAME + HINT_PATHNAME_SIZE];
	int filesize;
	ssize_t n = 0;
	size_t r_buffer = 0;
	
	// read status filename filesize
	int count = 0;
	while (n == 0 || (count < 3 && file_info[r_buffer - 1] != '\n')) {
		n = read(sockets_ds->fd_tcp, file_info + r_buffer, 1);
		if (n == ERROR) {
			printf(ERROR_SEND_TCP);
			return ERROR;
		}
		r_buffer += n;
		if (file_info[r_buffer - 1] == ' ') count++;
	}

	sscanf(file_info, "%s %s %d ", status, filename, &filesize);

	if (strcmp(status, "NOK") == EQUAL) {
		printf("The server has no hints for you. Sorry.\n");
		return ERROR;
	}
	
	char* filedata = (char*) malloc(sizeof(char) * (filesize));
	
	
	// read filedata
	r_buffer = 0;
	while (r_buffer < filesize) {
		n = read(sockets_ds->fd_tcp, filedata + r_buffer, filesize - r_buffer);
		if (n == ERROR) {
			printf(ERROR_SEND_TCP);
			return ERROR;
		}
		r_buffer += n;
	}

	sprintf(game_stats->hint_filename, "%s%s", HINT_PATHNAME, filename);
	FILE* file = fopen(game_stats->hint_filename, "w");
	if (file == NULL) return ERROR;
	
	for (int i = 0; i < filesize; i++) {
	    if (fputc(filedata[i], file) == EOF)
            return ERROR;
    }

	free(filedata);
	fclose(file);
	return filesize;
}

int send_hint_request(socket_ds* sockets_ds, optional_args opt_args, game_status* game_stats) {
	char request[HINT_REQUEST_SIZE];
	char code[4];
	ssize_t n;
	
	// prepare request
	if(game_stats->running == NO) {
		printf("No game running.\n");
		return ERROR;
	}
	sprintf(request, "GHL %s\n", game_stats->player_id);
	
	tcp_setup(sockets_ds, opt_args);

	// send request over to the server
	n = write(sockets_ds->fd_tcp, request, HINT_REQUEST_SIZE);
	if (n != HINT_REQUEST_SIZE) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}
	
	// receive the response from the previous request
	n = read(sockets_ds->fd_tcp, code, 4);
	if (n != 4) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}

	// turn code into a string
	code[3] = '\0';
	if (strcmp(code, "RHL") != EQUAL) {
		printf("ERRO\n");
		return ERROR;
	}

	int hint_filesize = process_hint_response(sockets_ds, game_stats);
	
	freeaddrinfo(sockets_ds->addrinfo_tcp_ptr);
	close(sockets_ds->fd_tcp);
	
	return hint_filesize;




}


int process_state_response(socket_ds* sockets_ds, game_status* game_stats) {

	char file_info[MAX_FILE_INFO_SIZE];
	char filename[MAX_FILENAME + STATE_PATHNAME_SIZE];
	int filesize;
	ssize_t n = 0;
	size_t r_buffer = 0;
	
	// read status filename filesize
	int count = 0;
	while (n == 0 || (count < 3 && file_info[r_buffer - 1] != '\n')) {
		n = read(sockets_ds->fd_tcp, file_info + r_buffer, 1);
		if (n == ERROR) {
			printf(ERROR_SEND_TCP);
			return ERROR;
		}
		r_buffer += n;
		if (file_info[r_buffer - 1] == ' ') count++;
	}

	sscanf(file_info, "%s %s %d ", game_stats->state_status, filename, &filesize);

	if (strcmp(game_stats->state_status, "NOK") == EQUAL) {
		printf("Error. Invalid player ID or no games (active or finished) for that player in the server.\n");
		return ERROR;
	}
	
	char* filedata = (char*) malloc(sizeof(char) * (filesize));
	
	
	// read filedata
	r_buffer = 0;
	while (r_buffer < filesize) {
		n = read(sockets_ds->fd_tcp, filedata + r_buffer, filesize - r_buffer);
		if (n == ERROR) {
			printf(ERROR_SEND_TCP);
			return ERROR;
		}
		r_buffer += n;
	}

	FILE* file = fopen(game_stats->state_filename, "w");
	if (file == NULL) {
		printf("Error. Couldn't save file.");
		return ERROR;
	}
	for (int i = 0; i < filesize; i++) {
	    if (fputc(filedata[i], file) == EOF)
            return ERROR;
    }

	free(filedata);
	fclose(file);
	
	return SUCCESS;
}


int send_state_request(socket_ds* sockets_ds, optional_args opt_args, game_status* game_stats) {
	char request[HINT_REQUEST_SIZE];
	char code[4];
	ssize_t n;
	
	// prepare request
	sprintf(request, "STA %s\n", game_stats->player_id);
	
	tcp_setup(sockets_ds, opt_args);

	// send request over to the server
	n = write(sockets_ds->fd_tcp, request, STATE_REQUEST_SIZE);
	if (n != STATE_REQUEST_SIZE) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}
	
	// receive the response from the previous request
	n = read(sockets_ds->fd_tcp, code, 4);
	if (n != 4) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}

	// turn code into a string
	code[3] = '\0';
	if (strcmp(code, "RST") != EQUAL) {
		printf("ERRO\n");
		return ERROR;
	}

	int res = process_state_response(sockets_ds, game_stats);
	
	freeaddrinfo(sockets_ds->addrinfo_tcp_ptr);
	close(sockets_ds->fd_tcp);
	
	return res;
}




