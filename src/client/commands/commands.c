/*
 * File: commands.c
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: functions to make requests to a remote word game server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "../player.h"
#include "commands.h"

/*--------------------UDP Module---------------*/
/*---------------Function prototypes---------------*/

int process_start_response(char*, ssize_t, game_status*);
int process_play_response(char*, ssize_t, game_status*);
int process_guess_response(char*, ssize_t, game_status*);
int process_quit_response(char*, ssize_t, game_status*);

/*---------------Functions---------------*/

//Sets up a UDP connection using opt_agrs and stores its information in sockets_ds
void udp_setup(socket_ds *sockets_ds, optional_args opt_args) {
	//opens a UDP socket
    sockets_ds->fd_udp = socket(AF_INET, SOCK_DGRAM, AUTO_PROTOCOL);
    if(sockets_ds->fd_udp == ERROR) {
        fprintf(stderr, ERROR_FD_UDP);
        exit(EXIT_FAILURE);
    }

    memset(&sockets_ds->addrinfo_udp, 0, sizeof(sockets_ds->addrinfo_udp));
    //set hints
    sockets_ds->addrinfo_udp.ai_family = AF_INET; //IPv4
    sockets_ds->addrinfo_udp.ai_socktype = SOCK_DGRAM;  //UDP socket
    
    //gets server info
    int ret = getaddrinfo(opt_args.ip, opt_args.port, &sockets_ds->addrinfo_udp, &sockets_ds->addrinfo_udp_ptr);
    if(ret != SUCCESS) {
        // Failed to get an internet address
		cleanup_connection(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr);
		fprintf(stderr, ERROR_ADDR_UDP);
		exit(EXIT_FAILURE);
    }
}


//Sends start request to game server
int send_start_request(socket_ds* sockets_ds, game_status* game_stats) {

	char player_id[MAX_STRING];
	char request[START_REQUEST_SIZE];
	char response[START_RESPONSE_SIZE];
	int ret;
	ssize_t ret_send_udp_request, ret_recv_udp_response;
	socklen_t addrlen;
	struct sockaddr_in addr;
	addrlen = sizeof(addr);

	//prepare request
	get_word(player_id);
	strcpy(game_stats->player_id, player_id);
	sprintf(request, "SNG %s\n", player_id);

	//send request over to the server
	ret_send_udp_request = sendto(sockets_ds->fd_udp, request, START_REQUEST_SIZE, 0, sockets_ds->addrinfo_udp_ptr->ai_addr, sockets_ds->addrinfo_udp_ptr->ai_addrlen);
	if(ret_send_udp_request == ERROR) {
		printf(ERROR_SEND_UDP);
		return ERROR;
	}

	//receive the response from the server
	ret_recv_udp_response = recvfrom(sockets_ds->fd_udp, response, START_RESPONSE_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(ret_recv_udp_response == ERROR) {
		printf(ERROR_RECV_UDP);
		return ERROR;
	}
	
	return process_start_response(response, ret_recv_udp_response, game_stats);
}


//Process the response from the game server to a start request
int process_start_response(char* response, ssize_t ret_recv_udp_response, game_status* game_stats) {
	//turn response into string
	if(ret_recv_udp_response < START_RESPONSE_SIZE) {
		response[ret_recv_udp_response] = '\0';
	}
	else {
		response[START_RESPONSE_SIZE - 1] = '\0';
	}
	
	//positive response
	if(strcmp(strtok(response, " "), "RSG") == EQUAL && strcmp(strtok(NULL, " "), "OK") == EQUAL) {
		game_stats->n_letters = atoi(strtok(NULL, " "));
		game_stats->n_errors = atoi(strtok(NULL, " "));
		game_stats->running = YES;
		game_stats->trial = 1;
		game_stats->word = (char*) malloc(sizeof(char) * game_stats->n_letters);
		for (int i = 0; i <= game_stats->n_letters - 1; i++) {
			game_stats->word[i] = '_';
		}
	}
	//error
	else {
		game_stats->running = MAYBE;
		printf(START_REQUEST_ERROR);
		return ERROR;
	}
	
	return SUCCESS;
}


//Sends play request to game server
int send_play_request(socket_ds* sockets_ds, game_status* game_stats) {

	char letter[MAX_STRING];
	char request[PLAY_REQUEST_SIZE];
	char response[PLAY_RESPONSE_SIZE];
	int ret;
	ssize_t ret_send_udp_request, ret_recv_udp_response;
	socklen_t addrlen;
	struct sockaddr_in addr;
	addrlen = sizeof(addr);
	
	//check if client has a game running
	if(game_stats->running == NO) {
		printf(NO_GAME_ERROR);
		return ERROR;
	}
	
	//prepare request
	get_word(letter);
	game_stats->last_letter = toupper(letter[0]);
	sprintf(request, "PLG %s %s %d\n", game_stats->player_id, letter, game_stats->trial);
	
	//send request over to the server
	int request_size = PLAY_REQUEST_SIZE;
	if(game_stats->trial > 9)
		//in case trial has more than 1 digit
		request_size++;	
	ret_send_udp_request = sendto(sockets_ds->fd_udp, request, request_size, 0, sockets_ds->addrinfo_udp_ptr->ai_addr, sockets_ds->addrinfo_udp_ptr->ai_addrlen);
	if(ret_send_udp_request == ERROR) {
		printf(ERROR_SEND_UDP);
		return ERROR;
	}

	//receive the response from the server
	ret_recv_udp_response = recvfrom(sockets_ds->fd_udp, response, PLAY_RESPONSE_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(ret_recv_udp_response == ERROR) {
		printf(ERROR_RECV_UDP);
		return ERROR;
	}
	response[ret_recv_udp_response] = '\0';

	return process_play_response(response, ret_recv_udp_response, game_stats);
}


//Process the response from the game server to a play request
int process_play_response(char* response, ssize_t ret_recv_udp_response, game_status* game_stats) {
	//turn response into string
	if(ret_recv_udp_response < PLAY_RESPONSE_SIZE) {
		response[ret_recv_udp_response] = '\0';
	}
	else {
		response[PLAY_RESPONSE_SIZE - 1] = '\0';
	}

	//process response
	//error
	if(strcmp(strtok(response, " "), "RLG") != EQUAL) {
		game_stats->last_play = ERR;
		printf(PLAY_REQUEST_ERROR);
		return ERROR;
	}
	
	char* status_code = strtok(NULL, " ");
	//correct play
	if(strcmp(status_code, "OK") == EQUAL && atoi(strtok(NULL, " ")) == game_stats->trial) {
		game_stats->last_play = OK;
		int n = atoi(strtok(NULL, " "));
		for(; n > 0; n--) {
			game_stats->word[atoi(strtok(NULL, " ")) - 1] = game_stats->last_letter;
		}
		game_stats->trial += 1;
	}
	//won the game
	else if(strcmp(status_code, "WIN") == EQUAL) {
		game_stats->last_play = WIN;
		for(int i = 0; i < game_stats->n_letters; i++) {
			if(game_stats->word[i] == '_'){
				game_stats->word[i] = game_stats->last_letter;
			}
		}
	}
	//duplicate play
	else if(strcmp(status_code, "DUP") == EQUAL) {
		game_stats->last_play = DUP;
	}
	//incorrect play
	else if(strcmp(status_code, "NOK") == EQUAL) {
		game_stats->trial += 1;
		game_stats->last_play = NOK;
	}
	//lost the game
	else if(strcmp(status_code, "OVR") == EQUAL) {
		game_stats->last_play = OVR;
	}
	//invalid trial number
	else if(strcmp(status_code, "INV") == EQUAL) {
		game_stats->last_play = INV;
		printf(INVALID_TRIAL_ERROR);
		return ERROR;
	}
	
	return SUCCESS;
}


//Sends guess request to game server
int send_guess_request(socket_ds* sockets_ds, game_status* game_stats) {

	char word[MAX_STRING];
	char request[GUESS_REQUEST_SIZE];
	char response[GUESS_RESPONSE_SIZE];
	int ret;
	ssize_t ret_send_udp_request, ret_recv_udp_response;
	socklen_t addrlen;
	struct sockaddr_in addr;
	addrlen = sizeof(addr);

	//prepare request
	get_word(word);
	memset(request,'\0',sizeof(request));
	game_stats->guess = word;
	sprintf(request, "PWG %s %s %d\n",game_stats->player_id, word, game_stats->trial);

	//send request over to the server
	ret_send_udp_request = sendto(sockets_ds->fd_udp, request, strlen(request), 0, sockets_ds->addrinfo_udp_ptr->ai_addr, sockets_ds->addrinfo_udp_ptr->ai_addrlen);
	if(ret_send_udp_request == ERROR) {
		printf(ERROR_SEND_UDP);
		return ERROR;
	}

	//receive the response from the server
	ret_recv_udp_response = recvfrom(sockets_ds->fd_udp, response, GUESS_RESPONSE_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(ret_recv_udp_response == ERROR) {
		printf(ERROR_RECV_UDP);
		return ERROR;
	}
	
	return process_guess_response(response, ret_recv_udp_response, game_stats);
}


//Process the response from the game server to a guess request
int process_guess_response(char* response, ssize_t ret_recv_udp_response, game_status* game_stats) {
	//turn response into string
	if(ret_recv_udp_response < GUESS_RESPONSE_SIZE) {		
		response[ret_recv_udp_response] = '\0';
	}
	else {
		response[GUESS_RESPONSE_SIZE - 1] = '\0';		
	}

	//process response
	//error
	if(strcmp(strtok(response, " "), "RWG") != EQUAL) {
		game_stats->last_play = ERR;
		// TO DO: mensagem de erro
		printf(GUESS_REQUEST_ERROR);
		return ERROR;
	}

	char* status_code = strtok(NULL, " ");
	//correct guess
	if(strcmp(status_code, "WIN") == EQUAL) {
		game_stats->last_play = WIN;
	}
	//incorrect guess
	else if(strcmp(status_code, "NOK") == EQUAL) {
		game_stats->trial += 1;
		game_stats->last_play = NOK;
	}
	//lost the game
	else if(strcmp(status_code, "OVR") == EQUAL) {
		game_stats->last_play = OVR;
	}
	//invalid trial number
	else if(strcmp(status_code, "INV") == EQUAL) {
		game_stats->last_play = INV;
		printf(INVALID_TRIAL_ERROR);
		return ERROR;
	}
	
	return SUCCESS;
}


//Sends quit request to game server
int send_quit_request(socket_ds* sockets_ds, game_status* game_stats) {

	char request[START_REQUEST_SIZE];
	char response[START_RESPONSE_SIZE];
	int ret;
	ssize_t ret_send_udp_request, ret_recv_udp_response;
	socklen_t addrlen;
	struct sockaddr_in addr;
	addrlen = sizeof(addr);
	
	//check if client has a game running
	if(game_stats->running == NO) {
		printf(NO_GAME_ERROR);
		return ERROR;
	}
	
	//prepare request
	sprintf(request, "QUT %s\n", game_stats->player_id);

	//send request over to the server
	ret_send_udp_request = sendto(sockets_ds->fd_udp, request, QUIT_REQUEST_SIZE, 0, sockets_ds->addrinfo_udp_ptr->ai_addr, sockets_ds->addrinfo_udp_ptr->ai_addrlen);
	if(ret_send_udp_request == ERROR) {
		printf(ERROR_SEND_UDP);
		return ERROR;
	}

	//receive the response from the server
	ret_recv_udp_response = recvfrom(sockets_ds->fd_udp, response, QUIT_RESPONSE_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(ret_recv_udp_response == ERROR) {
		printf(ERROR_RECV_UDP);
		return ERROR;
	}
	
	return process_quit_response(response, ret_recv_udp_response, game_stats);
}


//Process the response from the game server to a quit request
int process_quit_response(char* response, ssize_t ret_recv_udp_response, game_status* game_stats) {
	//turn response into str
	if(ret_recv_udp_response < START_RESPONSE_SIZE) {
		response[ret_recv_udp_response] = '\0';
	}
	else {
		response[START_RESPONSE_SIZE - 1] = '\0';
	}
	
	//process response
	if (strcmp(strtok(response, " "), "RQT") != EQUAL) {
		printf(ERROR_RECV_UDP);
		return ERROR;
	}
	
	game_stats->running = NO;
	
	if (strcmp(strtok(NULL, "\n"), "ERR") == EQUAL) {
		printf(QUIT_REQUEST_ERROR);
		return ERROR;
	}
	
	return SUCCESS;
}


/*--------------------TCP Module---------------*/
/*---------------Function prototypes---------------*/

int process_scoreboard_response(socket_ds*, game_status*);
int process_hint_response(socket_ds*, game_status*);
int process_state_response(socket_ds*, game_status*);

/*---------------Functions---------------*/

//Sets up a TCP connection using opt_agrs and stores its information in sockets_ds
void tcp_setup(socket_ds *sockets_ds, optional_args opt_args) {
	//opens a TCP socket
    sockets_ds->fd_tcp = socket(AF_INET, SOCK_STREAM, AUTO_PROTOCOL);
	if(sockets_ds->fd_tcp == ERROR) {
		fprintf(stderr,ERROR_FD_TCP);
		exit(EXIT_FAILURE);
	}

	memset(&sockets_ds->addrinfo_tcp, 0, sizeof(sockets_ds->addrinfo_tcp));
    //set hints
    sockets_ds->addrinfo_tcp.ai_family = AF_INET; //IPv4
    sockets_ds->addrinfo_tcp.ai_socktype = SOCK_STREAM;  //TCP socket
    
    //gets server info
    int ret = getaddrinfo(opt_args.ip, opt_args.port, &sockets_ds->addrinfo_tcp, &sockets_ds->addrinfo_tcp_ptr);
	if(ret != SUCCESS) {
        //failed to get an internet address
		cleanup_connection(sockets_ds->fd_tcp,sockets_ds->addrinfo_tcp_ptr);
		fprintf(stderr, ERROR_ADDR_TCP);
		exit(EXIT_FAILURE);

    }
 	
 	//connects with server
	ret = connect(sockets_ds->fd_tcp, sockets_ds->addrinfo_tcp_ptr->ai_addr, sockets_ds->addrinfo_tcp_ptr->ai_addrlen);
    if(ret == ERROR) {
		cleanup_connection(sockets_ds->fd_tcp,sockets_ds->addrinfo_tcp_ptr);
		fprintf(stderr, ERROR_TCP_CONNECT);
		exit(EXIT_FAILURE);
	}
}


//Sends scoreboard request to game server
int send_scoreboard_request(socket_ds* sockets_ds, optional_args opt_args, game_status* game_stats) {
	
	char code[4];
	ssize_t n;
	
	//opens tcp connection
	tcp_setup(sockets_ds, opt_args);

	//send request over to the server
	n = write(sockets_ds->fd_tcp, "GSB\n", SCOREBOARD_REQUEST_SIZE);
	if(n != SCOREBOARD_REQUEST_SIZE) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}
	
	//receive the response from the server
	n = read(sockets_ds->fd_tcp, code, 4);
	if(n != 4) {
		printf(ERROR_RECV_TCP);
		return ERROR;
	}

	//turn code into a string
	code[3] = '\0';
	if(strcmp(code, "RSB") != EQUAL) {
		printf(ERROR_RECV_TCP);
		return ERROR;
	}

	int res = process_scoreboard_response(sockets_ds, game_stats);

	cleanup_connection(sockets_ds->fd_tcp,sockets_ds->addrinfo_tcp_ptr);
	
	return res;
}


//Process the response from the game server to a scoreboard request
int process_scoreboard_response(socket_ds* sockets_ds, game_status* game_stats) {

	char file_info[MAX_FILE_INFO_SIZE];
	char filename[MAX_FILENAME + SCOREBOARD_PATHNAME_SIZE];
	int filesize;
	char status[6];
	ssize_t n = 0;
	size_t r_buffer = 0;
	
	//read status filename filesize
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
	
	//check if scoreboard is empty
	if (strcmp(status, "EMPTY") == EQUAL) {
		printf(EMPTY_SCOREBOARD_ERROR);
		return ERROR;
	}
	
	char* filedata = (char*) malloc(sizeof(char) * (filesize));
	
	//read filedata
	r_buffer = 0;
	while (r_buffer < (size_t)filesize) {
		n = read(sockets_ds->fd_tcp, filedata + r_buffer, filesize - r_buffer);
		if (n == ERROR) {
			printf(ERROR_SEND_TCP);
			return ERROR;
		}
		r_buffer += n;
	}
	
	//open file
	sprintf(game_stats->scoreboard_filename, "%s%s", SCOREBOARD_PATHNAME, filename);
	FILE* file = fopen(game_stats->scoreboard_filename, "w");
	if (file == NULL) {
		printf(OPEN_FILE_ERROR);
		return ERROR;
	}
	
	//save file
	for (int i = 0; i < filesize; i++) {
	    if (fputc(filedata[i], file) == EOF) {
	    	printf(SAVE_FILE_ERROR);
            return ERROR;
        }
    }

	free(filedata);
	fclose(file);
	
	return SUCCESS;
}


//Sends hint request to game server
int send_hint_request(socket_ds* sockets_ds, optional_args opt_args, game_status* game_stats) {
	char request[HINT_REQUEST_SIZE];
	char code[4];
	ssize_t n;
	
	//check if client has a game running
	if(game_stats->running == NO) {
		printf(NO_GAME_ERROR);
		return ERROR;
	}
	
	//opens tcp connection
	tcp_setup(sockets_ds, opt_args);
	
	//prepare request
	sprintf(request, "GHL %s\n", game_stats->player_id);

	//send request over to the server
	n = write(sockets_ds->fd_tcp, request, HINT_REQUEST_SIZE);
	if (n != HINT_REQUEST_SIZE) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}
	
	//receive the response from the server
	n = read(sockets_ds->fd_tcp, code, 4);
	if (n != 4) {
		printf(ERROR_RECV_TCP);
		return ERROR;
	}

	//turn code into a string
	code[3] = '\0';
	if (strcmp(code, "RHL") != EQUAL) {
		printf(ERROR_RECV_TCP);
		return ERROR;
	}

	int hint_filesize = process_hint_response(sockets_ds, game_stats);

	cleanup_connection(sockets_ds->fd_tcp,sockets_ds->addrinfo_tcp_ptr);
	
	return hint_filesize;
}


//Process the response from the game server to a hint request
int process_hint_response(socket_ds* sockets_ds, game_status* game_stats) {

	char file_info[MAX_FILE_INFO_SIZE];
	char status[4];
	char filename[MAX_FILENAME];
	int filesize;
	ssize_t n = 0;
	size_t r_buffer = 0;
	
	//read status filename filesize
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
	
	//checks if there is a hint
	if (strcmp(status, "NOK") == EQUAL) {
		printf(NO_HINT_ERROR);
		return ERROR;
	}
	
	char* filedata = (char*) malloc(sizeof(char) * (filesize));
	
	
	//read filedata
	r_buffer = 0;
	while (r_buffer < (size_t)filesize) {
		n = read(sockets_ds->fd_tcp, filedata + r_buffer, filesize - r_buffer);
		if (n == ERROR) {
			printf(ERROR_SEND_TCP);
			return ERROR;
		}
		r_buffer += n;
	}

	//open file
	sprintf(game_stats->hint_filename, "%s%s", HINT_PATHNAME, filename);
	FILE* file = fopen(game_stats->hint_filename, "w");
	if (file == NULL) {
		printf(OPEN_FILE_ERROR);
		return ERROR;
	}
	
	//save file
	for (int i = 0; i < filesize; i++) {
	    if (fputc(filedata[i], file) == EOF) {
	    	printf(SAVE_FILE_ERROR);
            return ERROR;
        }
    }

	free(filedata);
	fclose(file);
	
	return filesize;
}


//Sends state request to game server
int send_state_request(socket_ds* sockets_ds, optional_args opt_args, game_status* game_stats) {
	char request[HINT_REQUEST_SIZE];
	char code[4];
	ssize_t n;
	
	//opens a tcp connection
	tcp_setup(sockets_ds, opt_args);
	
	//prepare request
	sprintf(request, "STA %s\n", game_stats->player_id);

	//send request over to the server
	n = write(sockets_ds->fd_tcp, request, STATE_REQUEST_SIZE);
	if (n != STATE_REQUEST_SIZE) {
		printf(ERROR_SEND_TCP);
		return ERROR;
	}
	
	//receive the response from the server
	n = read(sockets_ds->fd_tcp, code, 4);
	if (n != 4) {
		printf(ERROR_RECV_TCP);
		return ERROR;
	}

	//turn code into a string
	code[3] = '\0';
	if (strcmp(code, "RST") != EQUAL) {
		printf(ERROR_RECV_TCP);
		return ERROR;
	}

	int res = process_state_response(sockets_ds, game_stats);
	
	cleanup_connection(sockets_ds->fd_tcp,sockets_ds->addrinfo_tcp_ptr);
	
	return res;
}


//Process the response from the game server to a state request
int process_state_response(socket_ds* sockets_ds, game_status* game_stats) {

	char file_info[MAX_FILE_INFO_SIZE];
	char filename[MAX_FILENAME + STATE_PATHNAME_SIZE];
	int filesize;
	ssize_t n = 0;
	size_t r_buffer = 0;
	
	//read status filename filesize
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
	
	//checks if player has active games
	if (strcmp(game_stats->state_status, "NOK") == EQUAL) {
		printf(NO_SERVER_GAME_ERROR);
		return ERROR;
	}
	
	char* filedata = (char*) malloc(sizeof(char) * (filesize));
	
	//read filedata
	r_buffer = 0;
	while (r_buffer < (size_t)filesize) {
		n = read(sockets_ds->fd_tcp, filedata + r_buffer, filesize - r_buffer);
		if (n == ERROR) {
			printf(ERROR_SEND_TCP);
			return ERROR;
		}
		r_buffer += n;
	}
	
	//open file
	sprintf(game_stats->state_filename, "%s%s", STATE_PATHNAME, filename);
	FILE* file = fopen(game_stats->state_filename, "w");
	if (file == NULL) {
		printf(OPEN_FILE_ERROR);
		return ERROR;
	}
	
	//save file
	for (int i = 0; i < filesize; i++) {
	    if (fputc(filedata[i], file) == EOF) {
	    	printf(SAVE_FILE_ERROR);
            return ERROR;
        }
    }

	free(filedata);
	fclose(file);
	
	return SUCCESS;
}

// frees addrinfo and closes connection (fd)
void cleanup_connection(int fd,struct addrinfo *addr) {

	freeaddrinfo(addr);
	close(fd);
	
}
