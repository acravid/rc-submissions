#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "../GS.h"
#include "request.h"
#include "../data/data_handler.h"

player_info games[PLAYERID_MAX - PLAYERID_MIN];
bool verbose;
FILE* word_file;
FILE* hint_data_file;


void init_player_info(input_args args, FILE* file) {
	verbose = args.verbose_flag;
	word_file = file;
	
	for (int i = 0; i < PLAYERID_MAX - PLAYERID_MIN; i++) {
		games[i].trial = 0;
		games[i].successful_trials = 0;
		strcpy(games[i].word, "");
		strcpy(games[i].last_request, "");
		strcpy(games[i].last_response, "");
		strcpy(games[i].played_letters, "");
	}
}

bool valid_playerid(char* player_id) {
	int id = atoi(player_id);
    return (id >= PLAYERID_MIN && id <= PLAYERID_MAX) ? true : false;
}


bool valid_letter(char letter) {
    return (letter >= ASCII_A && letter <= ASCII_z) ? true : false;
}

bool word_tolower(char* word) {
	int len = (int) strlen(word);
	for (int i = 0; i < len; i++) {
		if (!valid_letter(word[i]))
			return false;
		word[i] = tolower(word[i]);
	}
	return true;
}

/*--------------------UDP Module---------------*/
/*---------------Function prototypes---------------*/
void udp_setup(socket_ds*, input_args);
void udp_request_handler(socket_ds*);
void udp_select_requests_handler(char*, size_t, char*);
void start_request_handler(char*, size_t, char*);
void start_game(char*);
void play_request_handler(char*, size_t, char*);
void play(char*, char*, char, char*);
void guess(char *player_id, char*, char*);
void guess_request_handler(char*, size_t, char*);
void quit_request_handler(char*, size_t, char*);

/*---------------Functions---------------*/

//Setup UDP socket
void udp_setup(socket_ds* sockets_ds, input_args args) {
	//opens a UDP socket
    if((sockets_ds->fd_udp = socket(AF_INET, SOCK_DGRAM, AUTO_PROTOCOL)) == ERROR) {
        fprintf(stderr, ERROR_FD_UDP);
        exit(EXIT_FAILURE);
    } 

    memset(&sockets_ds->addrinfo_udp, 0, sizeof(sockets_ds->addrinfo_udp));
    sockets_ds->addrinfo_udp.ai_family = AF_INET; //IPv4
    sockets_ds->addrinfo_udp.ai_socktype = SOCK_DGRAM;  //UDP socket
    sockets_ds->addrinfo_udp.ai_flags = AI_PASSIVE; //
    
    struct timeval timeout;
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    //set timer
    if(setsockopt(sockets_ds->fd_udp, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) != SUCCESS) {
		fprintf(stderr, ERROR_ADDR_UDP);
		exit(EXIT_FAILURE);
    }

    if(getaddrinfo(NULL,args.port,&sockets_ds->addrinfo_udp, &sockets_ds->addrinfo_udp_ptr) != SUCCESS) {
        fprintf(stderr, ERROR_ADDR_UDP);
		exit(EXIT_FAILURE);
    }

    if(bind(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr->ai_addr,sockets_ds->addrinfo_udp_ptr->ai_addrlen) == ERROR) {
        fprintf(stderr, ERROR_BIND_UDP);
		exit(EXIT_FAILURE);
    }
}


//Handles UDP requests by receiving, processing and answering them
void udp_request_handler(socket_ds* sockets_ds) {

    char buffer[CLIENT_UDP_MAX_REQUEST_SIZE];
    char reply[MAX_PLAY_REPLY_SIZE];
    ssize_t ret_udp_response, ret_udp_request;
    int timeout_count;
 
    memset(reply, '\0', sizeof(reply));
    
    struct sockaddr_in addr;
    socklen_t addrlen;

    while(true) {

        addrlen = sizeof(addr);

        //receive client message from socket
		ret_udp_request = recvfrom(sockets_ds->fd_udp, buffer, CLIENT_UDP_MAX_REQUEST_SIZE, AUTO_PROTOCOL, (struct sockaddr*) &addr, &addrlen);
		if(ret_udp_request == ERROR) {
			cleanup_connection(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr);
            fprintf(stderr,ERROR_RECV_FROM);
            exit(EXIT_FAILURE);
        }

        //turn request to string
        if (ret_udp_request < CLIENT_UDP_MAX_REQUEST_SIZE)
        	buffer[ret_udp_request] = '\0';
        else
        	buffer[MAX_PLAY_REPLY_SIZE - 1] = '\0';
        
        //print request
        if (verbose)
			printf("IP: %s Port: %u Sent: %s", inet_ntoa(addr.sin_addr), addr.sin_port, buffer);
		
		//process request
        udp_select_requests_handler(buffer, ret_udp_request, reply);

        // send reply back to client
        ret_udp_response = ERROR;
        timeout_count = 0;
		while (ret_udp_response == ERROR) {
			ret_udp_response = sendto(sockets_ds->fd_udp, reply, strlen(reply), AUTO_PROTOCOL, (struct sockaddr*)&addr,addrlen);
			if(ret_udp_response == ERROR && timeout_count == MAX_TIMEOUTS) {
				cleanup_connection(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr);
            	fprintf(stderr,ERROR_SEND_TO);
            	exit(EXIT_FAILURE);
			}
			else if (ret_udp_response == ERROR) {
				printf(TIMEOUT_SEND_UDP);
				timeout_count += 1;
			}
		}
    }
}


//Reads the first 3 chars of UDP request to send it to the correct handler
void udp_select_requests_handler(char *buffer, size_t len, char *reply) {
    if(strncmp(START_CODE, buffer, CODE_SIZE) == EQUAL) {
        start_request_handler(buffer,len,reply);
    } 
    else if(strncmp(PLAY_CODE, buffer, CODE_SIZE) == EQUAL) {
        play_request_handler(buffer,len,reply);
    }
    else if(strncmp(GUESS_CODE, buffer, CODE_SIZE) == EQUAL) {
        guess_request_handler(buffer,len,reply);
    }
    else if(strncmp(QUIT_CODE, buffer, CODE_SIZE) == EQUAL) {
        quit_request_handler(buffer,len,reply);
    }
    else {
        sprintf(reply, "%s\n", ERROR_REPLY_CODE);
    }
}


//Checks if start request is valid and writes the reply
void start_request_handler(char *buffer, size_t len, char *reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);
	else {

		//read player id
		char playerid[PLAYERID_SIZE + 1];

		sscanf(&buffer[CODE_SIZE], "%s", playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[len - 1] != '\n') 
			sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);
		
		//check if player is valid
		else if (!valid_playerid(playerid)) 
			sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);

		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN].trial > 1) 
			sprintf(reply,"%s %s\n", START_REPLY_CODE, NOK_REPLY_CODE);

		else {
			//checks if last response was lost
			if (strcmp(buffer, games[atoi(playerid) - PLAYERID_MIN].last_request) == EQUAL) 
				strcpy(reply, games[atoi(playerid) - PLAYERID_MIN].last_response);
				
			//starts the game
			else {
				start_game(playerid);
				sprintf(reply,"%s %s %d %d\n", START_REPLY_CODE, OK_REPLY_CODE, 
					games[atoi(playerid) - PLAYERID_MIN].n_letters,
					games[atoi(playerid) - PLAYERID_MIN].n_errors);
				strcpy(games[atoi(playerid) - PLAYERID_MIN].last_request, buffer);
				strcpy(games[atoi(playerid) - PLAYERID_MIN].last_response, reply);
			}
		}
	}
}


//Starts a game
void start_game(char *player_id) {

	char word[MAX_WORD_SIZE];
	char hint[MAX_HINT_FILE_NAME_LENGHT];
	char line[MAX_LINE_LENGTH];
	char write_info[MAX_LINE_LENGTH];

	fgets(line,MAX_LINE_LENGTH,word_file);

	sscanf(line, "%s %s", word, hint);
	sprintf(write_info,"%s %s\n",word,hint);

	// write selected word and its hint filename to player's ongoing game file
	write_game_play_to_file(player_id,write_info,START_CODE);
	
	int len = strlen(word);
	int playerid = atoi(player_id);
	games[playerid - PLAYERID_MIN].n_letters = len;
	if (len <= 6)
		games[playerid - PLAYERID_MIN].n_errors = 7;
	else if (len <= 10)
		games[playerid - PLAYERID_MIN].n_errors = 8;
	else
		games[playerid - PLAYERID_MIN].n_errors = 9;

	games[playerid - PLAYERID_MIN].trial = 1;
	strcpy(games[playerid - PLAYERID_MIN].word, word);
}


//Checks if play request is valid and writes the reply
void play_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	size_t request_size = CODE_SIZE + 1 + PLAYERID_SIZE + 5;
	if (request_size != len && request_size + 1 != len) //trial may have 2 digits
		sprintf(reply, "%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		char letter[2];
		char trial[3];
		sscanf(&buffer[CODE_SIZE], "%s %s %s", playerid, letter, trial);

		//check if parsing was successful
		if (playerid == NULL || letter == NULL || trial == NULL) 
			sprintf(reply, "%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);

		//check for spaces in the right places
		else if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != ' ' || 
			buffer[CODE_SIZE + 1 + PLAYERID_SIZE + 1 + 1] != ' ') 
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);
	
		//check if the message ends with \n
		else if (buffer[len - 1] != '\n')
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);
	
		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);	

		//check if letter is valid
		else if (!valid_letter(letter[0]))
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);
	
		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN].trial < 1)
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);

		else {
			//checks if last response was lost
			if (strcmp(buffer, games[atoi(playerid) - PLAYERID_MIN].last_request) == EQUAL) 
				strcpy(reply, games[atoi(playerid) - PLAYERID_MIN].last_response);
			//check if letter is duplicate
			else if (strchr(games[atoi(playerid) - PLAYERID_MIN].played_letters, tolower(letter[0])) != NULL)
				sprintf(reply,"%s %s %s\n", PLAY_REPLY_CODE, DUP_REPLY_CODE, trial);
			//check if trial is valid
			else if (atoi(trial) != games[atoi(playerid) - PLAYERID_MIN].trial)
				sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, INV_REPLY_CODE);
			//make play
			else {
	   			char res[MAX_PLAY_REPLY_SIZE];	
				play(playerid,letter,tolower(letter[0]), res);
				sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, res);
			}
			strcpy(games[atoi(playerid) - PLAYERID_MIN].last_request, buffer);
			strcpy(games[atoi(playerid) - PLAYERID_MIN].last_response, reply);
		}
	}
}


//Makes a play
void play(char *player_id,char *letter_buffer,char letter, char* res) {
	
	int playerid = atoi(player_id);

	// write play to player's file
	write_game_play_to_file(player_id,letter_buffer,PLAY_CODE);

	games[playerid - PLAYERID_MIN].trial += 1;
	float n_succ = 0;
	float n_trials = 0;
	
	//incorrect play
	if (strchr(games[playerid - PLAYERID_MIN].word, letter) == NULL) {
		games[playerid - PLAYERID_MIN].n_errors -= 1;
		int len = strlen(games[playerid - PLAYERID_MIN].played_letters);
		games[playerid - PLAYERID_MIN].played_letters[len] = letter;
		games[playerid - PLAYERID_MIN].played_letters[len + 1] = '\0';
		//lost
		if (games[playerid - PLAYERID_MIN].n_errors < 0) {
			sprintf(res, "%s %d", OVR_REPLY_CODE, games[playerid - PLAYERID_MIN].trial - 1);

			create_player_game_directory(player_id); 
			rename_and_move_player_file(player_id,TERMINATION_STATUS_FAIL,n_succ,n_trials);

			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;		
		}
		else
			sprintf(res, "%s %d", NOK_REPLY_CODE, games[playerid - PLAYERID_MIN].trial - 1);
	}
	// correct play
	else {
		int count = 0;
		int pos[MAX_WORD_SIZE];
		for (int i = 0; i < (int) strlen(games[playerid - PLAYERID_MIN].word); i++) {
			if (games[playerid - PLAYERID_MIN].word[i] == letter) {
				pos[count] = i + 1;
				count += 1;
			}
		}
		
		games[playerid - PLAYERID_MIN].successful_trials += 1;
		n_succ = games[playerid - PLAYERID_MIN].successful_trials;
		n_trials = games[playerid - PLAYERID_MIN].trial;
		games[playerid - PLAYERID_MIN].n_letters -= count;
		int len = strlen(games[playerid - PLAYERID_MIN].played_letters);
		games[playerid - PLAYERID_MIN].played_letters[len] = letter;
		games[playerid - PLAYERID_MIN].played_letters[len + 1] = '\0';
		
		// won
		if (games[playerid - PLAYERID_MIN].n_letters == 0) {
			sprintf(res, "%s %d", WIN_REPLY_CODE, games[playerid - PLAYERID_MIN].trial - 1);

			create_player_game_directory(player_id); 
			rename_and_move_player_file(player_id, TERMINATION_STATUS_WIN,n_succ,n_trials);
			
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;		
			
		}
		else {
			sprintf(res, "%s %d %d", OK_REPLY_CODE, games[playerid - PLAYERID_MIN].trial - 1, count);
			for (int i = 0; i < count; i++) {
				sprintf(&res[strlen(res)], " %d", pos[i]);
			}
		}
	}
}


//Checks if guess request is valid and writes the reply
void guess_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	size_t request_size = CODE_SIZE + 1 + PLAYERID_SIZE + 5; //not knowing the word size 1 is min
	if (request_size > len && request_size + 1 > len) //trial may have 2 digits
		sprintf(reply, "%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
	else {
		//read player id, word and trial
		char playerid[PLAYERID_SIZE + 1];
		char word[MAX_WORD_SIZE + 1];
		char trial[3];

		sscanf(&buffer[CODE_SIZE], "%s %s %s", playerid, word, trial);

		//check if parsing was successful
		if(playerid == NULL || word == NULL || trial == NULL)
			sprintf(reply, "%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);

		//check for spaces in the right places
		else if(buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != ' ') 
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);

		//check if the message ends with \n
		else if(buffer[len - 1] != '\n')  
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);

		//check if player is valid
		else if(!valid_playerid(playerid))
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);

		
		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN].trial < 1)
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);

		//check if word is valid and makes it lower case
		else if (!word_tolower(word))
	   		sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
	   		
		else {
	   		//checks if last response was lost
			if (strcmp(buffer, games[atoi(playerid) - PLAYERID_MIN].last_request) == EQUAL) 
				strcpy(reply, games[atoi(playerid) - PLAYERID_MIN].last_response);
			//check if trial is valid
			else if (atoi(trial) != games[atoi(playerid) - PLAYERID_MIN].trial) 
				sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, INV_REPLY_CODE);
	
			//make guess
	   		else {
	   			char res[MAX_GUESS_REPLY_SIZE];
				guess(playerid, word, res);
				sprintf(reply,"%s %s %s\n", GUESS_REPLY_CODE, res, trial);
			}
			strcpy(games[atoi(playerid) - PLAYERID_MIN].last_request, buffer);
			strcpy(games[atoi(playerid) - PLAYERID_MIN].last_response, reply);
		}
	}	
}


//Makes guess
void guess(char *player_id, char* word, char* res) {

	int playerid = atoi(player_id);

	// write guess to player's file
	write_game_play_to_file(player_id,word,GUESS_CODE);

	games[playerid - PLAYERID_MIN].trial += 1;
	float n_succ = 0;
	float n_trials = 0;

	
	// wrong guess
	if (strcmp(games[playerid - PLAYERID_MIN].word, word) != EQUAL) {
		games[playerid - PLAYERID_MIN].n_errors -= 1;
		//lost
		if (games[playerid - PLAYERID_MIN].n_errors < 0) {
			sprintf(res, "%s", OVR_REPLY_CODE);;
			
			create_player_game_directory(player_id); 
			rename_and_move_player_file(player_id,TERMINATION_STATUS_FAIL,n_succ,n_trials);
			
			games[playerid - PLAYERID_MIN].successful_trials = 0;
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;	
		}
		else
			sprintf(res, "%s", NOK_REPLY_CODE);
	}
	// right guess
	else {
		games[playerid - PLAYERID_MIN].successful_trials += 1;
		sprintf(res, "%s", WIN_REPLY_CODE);

		create_player_game_directory(player_id); 
		rename_and_move_player_file(player_id,TERMINATION_STATUS_WIN,n_succ,n_trials);
		
		games[playerid - PLAYERID_MIN].successful_trials = 0;
		games[playerid - PLAYERID_MIN].trial = 0;
		games[playerid - PLAYERID_MIN].played_letters[0] = '\0';	
	}
}


//Checks if quit request is valid and writes the reply
void quit_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, ERROR_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		if (sscanf(&buffer[CODE_SIZE], "%s", playerid) == EOF)
			sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, ERROR_REPLY_CODE);
		
		if (playerid == NULL)
			sprintf(reply,"%s\n", QUIT_REPLY_CODE, ERROR_REPLY_CODE);
			
		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n')
			sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, ERROR_REPLY_CODE);

		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, ERROR_REPLY_CODE);
			
		//check if player has a game running
		else if (games[atoi(playerid) - PLAYERID_MIN].trial == 0)
			sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, NOK_REPLY_CODE);
			
		//ends the game
		else {

			int n_succ = 0;
			int n_trials = 0;
			
			create_player_game_directory(playerid); 
			rename_and_move_player_file(playerid,TERMINATION_STATUS_QUIT,n_succ,n_trials);
				
			games[atoi(playerid) - PLAYERID_MIN].played_letters[0] = '\0';
			games[atoi(playerid) - PLAYERID_MIN].trial = 0;
			games[atoi(playerid) - PLAYERID_MIN].successful_trials = 0;
			strcpy(games[atoi(playerid) - PLAYERID_MIN].last_request, "");
			strcpy(games[atoi(playerid) - PLAYERID_MIN].last_response, "");
			
			sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, OK_REPLY_CODE);
		}
	}
}



/*--------------------TCP Module---------------*/
/*---------------Function prototypes---------------*/

void tcp_setup(socket_ds*, input_args);
void tcp_request_handler(socket_ds*);
void tcp_select_requests_handler(char*, size_t, char*);

void scoreboard_request_handler(char*, size_t, char*);
void scoreboard(char*);
void hint_request_handler(char*, size_t, char*);
void hint(char*, char*);
void state_request_handler(char*, size_t, char*);
void state(char*, char*, int, char*);

void cleanup_connection(int, struct addrinfo*);

/*---------------Functions---------------*/

//Setup TCP socket
void tcp_setup(socket_ds* sockets_ds, input_args args) {
	//opens a TCP socket
    if((sockets_ds->fd_tcp = socket(AF_INET, SOCK_STREAM, AUTO_PROTOCOL)) == ERROR) {
        fprintf(stderr, ERROR_FD_TCP);
        exit(EXIT_FAILURE);
    } 

    memset(&sockets_ds->addrinfo_tcp, 0, sizeof(sockets_ds->addrinfo_tcp));
    //set hints
    sockets_ds->addrinfo_tcp.ai_family = AF_INET; //IPv4
    sockets_ds->addrinfo_tcp.ai_socktype = SOCK_DGRAM;  //TCP socket
    sockets_ds->addrinfo_tcp.ai_flags = AI_PASSIVE; 
	
	struct timeval timeout;
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    //set timer
    if(setsockopt(sockets_ds->fd_tcp, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) != SUCCESS) 
		exit(EXIT_FAILURE);

    if(setsockopt(sockets_ds->fd_tcp, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != SUCCESS) 
		exit(EXIT_FAILURE);
    
    if(getaddrinfo(NULL,args.port,&sockets_ds->addrinfo_tcp, &sockets_ds->addrinfo_tcp_ptr) != SUCCESS) {
        fprintf(stderr, ERROR_ADDR_TCP);
		exit(EXIT_FAILURE);
    }

    if(bind(sockets_ds->fd_tcp,sockets_ds->addrinfo_tcp_ptr->ai_addr,sockets_ds->addrinfo_tcp_ptr->ai_addrlen) == ERROR) {
        fprintf(stderr, ERROR_BIND_TCP);
		exit(EXIT_FAILURE);
    }

    if(listen(sockets_ds->fd_tcp, MAX_QUEUED_REQUESTS) == ERROR) {
        fprintf(stderr, ERROR_LISTEN);
        exit(EXIT_FAILURE);
    }
}


//Handles TCP requests by creating a new process to receive, process and answer them
void tcp_request_handler(socket_ds* sockets_ds) {

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);;
    ssize_t ret_tcp_request, ret_tcp_response;
    size_t w_buffer;
    int timeout_count;
    char request[CLIENT_TCP_MAX_REQUEST_SIZE];
    char reply[SERVER_TCP_MAX_REPLY_SIZE];
    pid_t pid;
    
    while(true) {
		int newfd = ERROR;
		
        do newfd = accept(sockets_ds->fd_tcp, (struct sockaddr*)&addr, &addrlen);
        while((newfd == ERROR) && (errno = EINTR));
        if(newfd == ERROR) {
            cleanup_connection(sockets_ds->fd_tcp,sockets_ds->addrinfo_udp_ptr);
            fprintf(stderr,ERROR_SEND_TO);
            exit(EXIT_FAILURE);
        }
        
        // create a new child process for each new connection
        pid = fork();
        if (pid == ERROR) {
            fprintf(stderr,ERROR_FORK);
            exit(EXIT_FAILURE);
        } 
        else if (pid == FORK_CHILD) { 
            if (close(sockets_ds->fd_tcp) == ERROR) {
                fprintf(stderr, ERROR_CLOSE);
                exit(EXIT_FAILURE);
            }
            
            char code[4];
			char status[4];
			char name[MAX_FILENAME];
			char size[MAX_FILE_SIZE_DIGITS];
			char data[MAX_FILE_SIZE];
			
			//reads request
            ret_tcp_request = ERROR;
            timeout_count = 0;
            while (ret_tcp_request == ERROR) {
				ret_tcp_request = read(newfd, request, CLIENT_TCP_MAX_REQUEST_SIZE);
				if (ret_tcp_request == ERROR && timeout_count == MAX_TIMEOUTS) {
					fprintf(stderr, ERROR_READ);
                	close(newfd);
					exit(EXIT_FAILURE);	
				}
				else if (ret_tcp_request == ERROR) {
					printf(TIMEOUT_RECV_TCP);
					timeout_count += 1;
				}
			}
            
			//make request a string
			request[ret_tcp_request] = '\0';
			
			//print request
        	if (verbose)
				printf("IP: %s Port: %u Sent: %s", inet_ntoa(addr.sin_addr), addr.sin_port, request);
			
            // process request
			tcp_select_requests_handler(request, ret_tcp_request, reply);
			
			sscanf(reply, "%s %s %s %s", code, status, name, size);
			
			size_t len = strlen(reply);
			ret_tcp_request = ERROR;
            timeout_count = 0;
			w_buffer = 0;
			//write response
			while (ret_tcp_response == ERROR || w_buffer != len) {
				ret_tcp_response = write(newfd, reply, len - w_buffer);
				if (ret_tcp_response == ERROR && timeout_count == MAX_TIMEOUTS) {
					fprintf(stderr, ERROR_WRITE);
                	close(newfd);
					exit(EXIT_FAILURE);
				}
				else if (ret_tcp_response == ERROR) {
					printf(TIMEOUT_SEND_TCP);
					timeout_count += 1;
				}
				else
					w_buffer += ret_tcp_response;
			}
			
			//in case there is a hint image to send
			if (strcmp(status, "OK") == EQUAL && strcmp(code, "RHL") == EQUAL) {
				//read file data
				fread(data, atoi(size) + 1, 1, hint_data_file);
				fclose(hint_data_file);
				
				//write file data
				ret_tcp_request = ERROR;
            	timeout_count = 0;
				w_buffer = 0;
				len = (size_t) atoi(size);
				while (ret_tcp_response == ERROR || w_buffer != len) {
					ret_tcp_response = write(newfd, data, len - w_buffer);
					if (ret_tcp_response == ERROR && timeout_count == MAX_TIMEOUTS) {
						fprintf(stderr, ERROR_WRITE);
                		close(newfd);
						exit(EXIT_FAILURE);
					}
					else if (ret_tcp_response == ERROR) {
						printf(TIMEOUT_SEND_TCP);
						timeout_count += 1;
					}
					else
						w_buffer += ret_tcp_response;
				}
			}

			close(newfd);
			exit(EXIT_SUCCESS);	
        } 
    }
}


//Reads the first 3 chars of TCP request to send it to the correct handler
void tcp_select_requests_handler(char* request, size_t len, char* reply) {
    if(strncmp(SCOREBOARD_CODE, request, CODE_SIZE) == EQUAL) {
        scoreboard_request_handler(request, len, reply);
    } 
    else if(strncmp(HINT_CODE, request, CODE_SIZE) == EQUAL) {
        hint_request_handler(request, len, reply);
    }
    else if(strncmp(STATE_CODE, request, CODE_SIZE) == EQUAL) {
        state_request_handler(request, len, reply);
    }
     else {
        sprintf(reply, "%s\n", ERROR_REPLY_CODE);
    }
}


//Checks if scoreboard request is valid and writes the reply
void scoreboard_request_handler(char* request, size_t len, char* reply) {	
	//check if request is correct
	if (strcmp("GSB\n", request) != EQUAL)
		sprintf(reply, "%s %s\n", SCOREBOARD_REPLY_CODE, ERROR_REPLY_CODE);
	else 
		scoreboard(reply);
}


//Makes and sends the scoreboard
void scoreboard(char* reply) {
	
	char scoreboard_path_name[strlen(SCORES_DATA_DIR) + strlen(SCOREBOARD_FILE) + 1];
	char data[MAX_FILE_SIZE];
	int size = 0;
	
	//open file
	sprintf(scoreboard_path_name, "%s/%s", SCORES_DATA_DIR, SCOREBOARD_FILE);
	FILE* scoreboard_file = fopen(scoreboard_path_name, "r");
	
	//read scoreboard data
	for (char c = fgetc(scoreboard_file); c != EOF; c = fgetc(scoreboard_file), size += 1)
		data[size] = c;
	
	if (size == 0)
		sprintf(reply, "%s %s\n", SCOREBOARD_REPLY_CODE, EMPTY_REPLY_CODE);
	else
		sprintf(reply, "%s %s %s %d %s", SCOREBOARD_REPLY_CODE, OK_REPLY_CODE, SCOREBOARD_FILE, size, data);
		
	fclose(scoreboard_file);
}


//Checks if hint request is valid and writes the reply
void hint_request_handler(char* buffer, size_t len, char* reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s %s\n", HINT_REPLY_CODE, NOK_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		sscanf(&buffer[CODE_SIZE], "%s", playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n') 
			sprintf(reply,"%s %s\n", HINT_REPLY_CODE, NOK_REPLY_CODE);

		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s %s\n", HINT_REPLY_CODE, NOK_REPLY_CODE);
		
		//send hint
		else {
			char hint_name[MAX_FILENAME];
			get_hint_filename(hint_name, playerid);
			if (strcmp(hint_name, NO_HINT) == EQUAL) 
				sprintf(reply,"%s %s\n", HINT_REPLY_CODE, NOK_REPLY_CODE);
			else {
				char res[MAX_HINT_REPLY_SIZE];
				hint(hint_name, res);
				sprintf(reply, "%s %s %s", HINT_REPLY_CODE, OK_REPLY_CODE, res);
			}
		}
	}
}


//Gets he information for sending the a hint image
void hint(char* hint_name, char* reply) {
	char hint_path_name[strlen(hint_name) + strlen(HINT_FILE_PATH)];
	int size = 0;
	
	//close previous hint file
	if (hint_data_file != NULL)
		fclose(hint_data_file);
	
	//open new hint file
	sprintf(hint_path_name, "%s%s", HINT_FILE_PATH, hint_name);
	hint_data_file = fopen(hint_path_name, "r");
	
	//read the size of the image
	fseek(hint_data_file, 0L, SEEK_END);
	size = ftell(hint_data_file);
	rewind(hint_data_file);
	
	sprintf(reply, "%s %d ", hint_name, size);
}


//Checks if state request is valid and writes the reply
void state_request_handler(char* buffer, size_t len, char *reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s %s\n", STATE_REPLY_CODE, NOK_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		sscanf(&buffer[CODE_SIZE], "%s", playerid);
		int plid = atoi(playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n') 
			sprintf(reply,"%s %s\n", STATE_REPLY_CODE, NOK_REPLY_CODE);

		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s %s\n", STATE_REPLY_CODE, NOK_REPLY_CODE);

		else {
			char state_filename[MOVED_PLAY_FILE_LENGTH];
			char code[3];
			get_state_filename(playerid, state_filename, code);
			
			//checks if a state file exists
			if (state_filename == NULL) 
				sprintf(reply,"%s %s\n", STATE_REPLY_CODE, NOK_REPLY_CODE);
			//open and read file
			else {
				char res[MAX_STATE_REPLY_SIZE];
				state(state_filename, res, plid,code);
				
				sprintf(reply, "%s %s %s\n", STATE_REPLY_CODE, code, res);
			}
		}
	}
}


//Sends the state file data
void state(char* state_path,char* reply,int playerid,char *code) {

	char data[MAX_FILE_SIZE];
	int size = 0;
	
	//open file
	FILE* state_file = fopen(state_path,"r");
	
	//checks if it's an active game
	if(strcmp(code, STATE_ACTIVE) == SUCESS) {
		//doesn't send the first line from state
		char *line_to_ignore = NULL;
		line_to_ignore = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);
		fgets(line_to_ignore,MAX_LINE_LENGTH,state_file);
		free(line_to_ignore);
	}

	//read file
	for (char c = fgetc(state_file); c != EOF; c = fgetc(state_file), size += 1)
		data[size] = c;
	
	memset(reply,sizeof(reply), '\0');
	sprintf(reply, "STATE_%d.txt %d %s", playerid, size, data);
	
	fclose(state_file); 
}


//Frees addrinfo and closes connection
void cleanup_connection(int fd,struct addrinfo *addr) {
	freeaddrinfo(addr);
	close(fd);
}