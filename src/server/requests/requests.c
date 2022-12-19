#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "request.h"
#include "../GS.h"

player_info games[PLAYERID_MAX - PLAYERID_MIN];

//--------------------------------------------------------------
//                  UDP Module                                  
//--------------------------------------------------------------
void init_player_info() {
	for (int i = 0; i < PLAYERID_MAX - PLAYERID_MIN; i++) {
		games[i].trial = 0;
		strcpy("", games[i].word);
		strcpy("", games[i].played_letters);
	}
}

bool valid_playerid(char* player_id) {
	int id = atoi(player_id);
    return (id >= PLAYERID_MIN && id <= PLAYERID_MAX) ? true : false;

}


bool valid_letter(char* letter) {
	char l = letter[0];
    return (l >= ASCII_A && l <= ASCII_z) ? true : false;
	
}

void start_game(int playerid) {
	char word[MAX_WORD_SIZE];

	//TODO:read word
	
	int len = strlen(word);
	games[playerid - PLAYERID_MIN].n_letters = len;
	if (len <= 6)
		games[playerid - PLAYERID_MIN].n_errors = 7;
	else if (len <= 10)
		games[playerid - PLAYERID_MIN].n_errors = 8;
	else
		games[playerid - PLAYERID_MIN].n_errors = 9;


	games[playerid - PLAYERID_MIN].trial = 1;
	strcpy(word, games[playerid - PLAYERID_MIN].word);
}

void start_request_handler(char *buffer, size_t len, char *reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		sscanf(&buffer[CODE_SIZE], "%s", playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n') 
			sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);

		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);

		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN].trial > 1)
			sprintf(reply,"%s %s\n", START_REPLY_CODE, NOK_REPLY_CODE);

		//starts the game
		else {
			start_game(atoi(playerid));
			sprintf(reply,"%s %s %d %d\n", START_REPLY_CODE, OK_REPLY_CODE, 
					games[int_playerid - PLAYERID_MIN].n_letters,
					games[int_playerid - PLAYERID_MIN].n_errors);
		}
	}
}

void play(int playerid, char letter, char* res) {

	games[playerid - PLAYERID_MIN].trial += 1;

	if (strchr(pigames[playerid - PLAYERID_MIN].word, letter) == NULL) {
		games[playerid - PLAYERID_MIN].n_errors -= 1;
		if (games[playerid - PLAYERID_MIN].n_errors < 0) {
			sprintf(res, "%s\n", OVR_REPLY_CODE);
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;
		}
		else
			sprintf(res, "%s\n", NOK_REPLY_CODE);
	}
	else {
		games[playerid - PLAYERID_MIN]n_letters -= 1;
		if (games[playerid - PLAYERID_MIN]n_letters = 0) {
			sprintf(res, "%s\n", WIN_REPLY_CODE);
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;
		}
		else {
			int count = 0;
			int pos[MAX_WORD_SIZE];
			for (int i = 0; i < strlen(games[playerid - PLAYERID_MIN]word); i++) {
				if (games[playerid - PLAYERID_MIN]word[i] == letter) {
					pos[count] = i;
					count += 1;
				}
			}
			sprintf(res, "%s %d", OK_REPLY_CODE, count);
			for (int i = 0, i < count; i++) {
				sprintf(&res[strlen(res)], " %d", pos[i]);
			}
		}
	}
}

void play_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	int request_size = CODE_SIZE + 1 + PLAYERID_SIZE + 5;
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
		else if (!valid_letter(letter))
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);

		//check if letter is duplicate
		else if (strchr(games[playerid - PLAYERID_MIN]->played_letters, letter[0]) != NULL)
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, DUP_REPLY_CODE);
	
		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN]->trial < 1)
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);

		//check if trial is valid
		else if (atoi(trial) != games[atoi(playerid) - PLAYERID_MIN]->trial)
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, INV_REPLY_CODE);

		//make play
		else {
	   		char res[MAX_PLAY_REPLY_SIZE];	
			play(atoi(playerid), letter[0], res);
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, res);
		}
	}
}

void guess(int playerid, char* word, char* res) {

	games[playerid - PLAYERID_MIN].trial += 1;

	if (strcmp(pigames[playerid - PLAYERID_MIN].word, word) != EQUAL) {
		games[playerid - PLAYERID_MIN].n_errors -= 1;
		if (games[playerid - PLAYERID_MIN].n_errors < 0) {
			sprintf(res, "%s\n", OVR_REPLY_CODE);
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;
		}
		else
			sprintf(res, "%s\n", NOK_REPLY_CODE);
	}
	else {
		games[playerid - PLAYERID_MIN]n_letters -= 1;
		if (games[playerid - PLAYERID_MIN]n_letters = 0) {
			sprintf(res, "%s\n", WIN_REPLY_CODE);
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;
		}
	}
}


void guess_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	int request_size = CODE_SIZE + 1 + PLAYERID_SIZE + 5; //not knowing the word size 1 is min
	if (request_size > len && request_size + 1 > len) //trial may have 2 digits
		sprintf(reply, "%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
	else {
		//read player id, word and trial
		char playerid[PLAYERID_SIZE + 1];
		char word[MAX_WORD_SIZE + 1];
		char trial[3];
		char letter[2];
		sscanf(&buffer[CODE_SIZE], "%s %s %s", playerid, letter, trial);

		//check if parsing was successful
		if(playerid == NULL || letter == NULL || trial == NULL)
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
		else if (games[atoi(playerid) - PLAYER_MIN]->trial < 1)
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);

		//check if trial is valid
		else if (atoi(trial) != games[atoi(playerid) - PLAYER_MIN]->trial)
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, INV_REPLY_CODE);

		//make play
		else {
	   		char res[MAX_GUESS_REPLY_SIZE];	
			guess(atoi(playerid), word, res);
			sprintf(reply,"%s %s %s\n", GUESS_REPLY_CODE, res, trial);
		}
	}	
}


void quit_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len)
		sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, ERROR_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		sscanf(&buffer[CODE_SIZE], "%s", playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n')
			sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, ERROR_REPLY_CODE);

		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, ERROR_REPLY_CODE);
		//ends the game
		else {
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;
			sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, OK_REPLY_CODE);
		}
	}
}


int udp_select_requests_handler(char *buffer, size_t len, char *reply) {
	
    if(strncmp(START_CODE, buffer, CODE_SIZE)) {
        start_request_handler(buffer,len,reply);
    } 
    else if(strncmp(PLAY_CODE, buffer, CODE_SIZE)) {
        play_request_handler(buffer,len,reply);
    }
    else if(strncmp(GUESS_CODE, buffer, CODE_SIZE)) {
        guess_request_handler(buffer,len,reply);
    }
    else if(strncmp(QUIT_CODE, buffer, CODE_SIZE)) {
        quit_request_handler(buffer,len,reply);
    }
    else {
        sprintf(reply, "%s\n", ERROR_REPLY_CODE);
    }
}


// handle requests to separate handlers
void udp_requests_handler(socket_ds* sockets_ds) {

    // having the udp socket completely set up
    // we can now process requests from clients
    char buffer[CLIENT_UDP_MAX_REQUEST_SIZE];
    char reply[MAX_PLAY_REPLY_SIZE];
 
    memset(reply, '\0', sizeof(reply));
    
    struct sockaddr_in addr;
    socklen_t addrlen;
    ssize_t n, nread;

    while(true) {

        addrlen = sizeof(addr);

        // receive client message from socket
        size_t nread = recvfrom(sockets_ds->fd_udp,buffer,CLIENT_UDP_MAX_REQUEST_SIZE,AUTO_PROTOCOL,(struct sockaddr*)&addr,addrlen);
        if(nread == ERROR) {
            cleanup_connection(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr);
            fprint(stderr,ERROR_RECV_FROM);
            exit(EXIT_FAILURE);
        }

        // protocol states that request and reply end with '\n'
        if(buffer[nread - 1] == '\n') {

            // turn request to string 
            buffer[nread - 1] = '\0';
            udp_select_requests_handler(buffer, nread, reply);
            
        } else {
            strcpy(reply, ERROR_REPLY_CODE);

        }

        // send reply back to client
        n = sendto(sockets_ds->fd_udp, reply, strlen(reply), AUTO_PROTOCOL, (struct sockaddr*)&addr,addrlen);
        if(n == ERROR) {
            cleanup_connection(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr);
            fprint(stderr,ERROR_SENDO_TO);
            exit(EXIT_FAILURE);
        }

    }

}

// set up UDP SOCKET 
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


// FIX ME: add macros for SIZE, HINT_REPLY_CODE 

void scoreboard_request_handler(char* request, size_t len, char* reply) {
	
	//check if request is correct
	if (strcmp("GSB\n", request) != EQUAL)
		sprintf(reply, "%s %s\n", SCOREBOARD_REPLY_CODE, ERROR_REPLY_CODE);
	
	//TODO scoreboard_empty()
	else if (scoreboard_empty())
		sprintf(reply, "%s %s\n", SCOREBOARD_REPLY_CODE, EMPTY_REPLY_CODE);
	//TODO scoreboard(), define sizes and haddle file open and read errors 
	else {
		char* filename_filesize[SIZE];
		char* filename[SIZE];
		char* filesize[SIZE];
		char* filedata[SIZE];
		scoreboard(filename_filesize);
		sscanf(filename_filesize, "%s %s", filename, filesize);
		
		//open and read file
		FILE* file = fopen(filename, "r");
		fread(filedata, SIZE, 1, file);

		sprintf(reply, "%s %s %s %s %s\n", SCOREBOARD_REPLY_CODE, OK_REPLY_CODE, filename, filesize, filedata);
	}
}


void hint_request_handler(char* buffer, size_t len, char* reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s %s\n", HINT_REPLY_CODE, ERROR_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		sscanf(&buffer[CODE_SIZE], "%s", playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n') 
			sprintf(reply,"%s %s\n", HINT_REPLY_CODE, ERROR_REPLY_CODE);

		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s %s\n", HINT_REPLY_CODE, ERROR_REPLY_CODE);

		//check if player has an ongoing game
		//TODO ongoing_game_w_or_wo_moves()
		else if (!ongoing_game_w_or_wo_moves(playerid))
			sprintf(reply,"%s %s\n", HINT_REPLY_CODE, NOK_REPLY_CODE);

		//TODO hint(), define sizes and haddle file open and read errors 
		else {
			char* filename_filesize[SIZE];
			char* filename[SIZE];
			char* filesize[SIZE];
			char* filedata[SIZE];
			hint(filename_filesize);
			sscanf(filename_filesize, "%s %s", filename, filesize);
		
			//open and read file
			FILE* file = fopen(filename, "r");
			fread(filedata, SIZE, 1, file);

			sprintf(reply, "%s %s %s %s %s\n", HINT_REPLY_CODE, OK_REPLY_CODE, filename, filesize, filedata);
		}
	}
}


void state_request_handler(char* buffer, size_t len, char *reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s %s\n", STATE_REPLY_CODE, ERROR_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		sscanf(&buffer[CODE_SIZE], "%s", playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n') 
			sprintf(reply,"%s %s\n", STATE_REPLY_CODE, ERROR_REPLY_CODE);

		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s %s\n", STATE_REPLY_CODE, ERROR_REPLY_CODE);

		//TODO state(), define sizes and haddle file open and read errors 
		else {
			char* status_filename_filesize[SIZE];
			char status[4];
			char* filename[SIZE];
			char* filesize[SIZE];
			char* filedata[SIZE];
			state(status_filename_filesize);
			sscanf(filename_filesize, "%s %s %s", status, filename, fileize);

			if (strcmp(NOK_REPLY_CODE, status) == EQUAL)
				sprintf(reply,"%s %s\n", STATE_REPLY_CODE, NOK_REPLY_CODE);
			else {
				//open and read file
				FILE* file = fopen(filename, "r");
				fread(filedata, SIZE, 1, file);

				sprintf(reply, "%s %s %s %s %s\n", HINT_REPLY_CODE, OK_REPLY_CODE, filename, filesize, filedata);
			}
		}
	}


}

void tcp_select_requests_handler(char* request, size_t len, char* reply) {

    if(strncmp(SCOREBOARD_CODE, request, CODE_SIZE)) {
        scoreboard_request_handler(request, len, reply);
    } 
    else if(strncmp(HINT_CODE, request, CODE_SIZE)) {
        hint_request_handler(request, len, reply);
    }
    else if(strncmp(STATE_CODE, request, CODE_SIZE)) {
        status_request_handler(request, len, reply);
    }
     else {
        sprintf(reply, "%s\n", ERROR_REPLY_CODE);
    }
}


ssize_t read_bytes_tcp(int fd, char *buffer, size_t number_of_bytes) {    

    // FIX ME 
    // add timeout (timer)

    ssize_t read_bytes = 0;
    int reading = 1;

    while (reading) {
        ssize_t already_read;
        already_read = read(fd,buffer,number_of_bytes - (size_t) read_bytes);
        reading = (already_read == ERROR) && (errno == EINTR);
        read_bytes += already_read;
        if(already_read == ERROR) {
            return ERROR;
        }
    }   
    return (ssize_t) read_bytes;
}


// handle requests to different functions
void tcp_requests_handler(socket_ds* sockets_ds) {

    int newfd;
    struct sockaddr_in addr;
    socklen_t addrlen;
    ssize_t n, nread;
    char request[CLIENT_TCP_MAX_REQUEST_SIZE];
    char reply[SERVER_TCP_MAX_REPLY_SIZE];
    pid_t pid;
 
    memset(reply,'\0', sizeof(reply));
    
    while(true) {

        addrlen = sizeof(addr);

        do newfd = accept(sockets_ds->fd_tcp,(struct sockaddr*)&addr,&addrlen);
        while((newfd == ERROR) && (errno = EINTR));

        if(newfd == ERROR) {
            cleanup_connection(sockets_ds->fd_tcp,sockets_ds->addrinfo_udp_ptr);
            fprint(stderr,ERROR_SENDO_TO);
            exit(EXIT_FAILURE);
        }
        // create a new child process for each new connection
        pid = fork();
        if(pid == ERROR) {
            fprint(stderr,ERROR_FORK);
            exit(EXIT_FAILURE);
        } 
        else if(pid == FORK_CHILD) { // child process
            if(close(sockets_ds->fd_tcp) == ERROR) {
                fprintf(stderr,ERROR_CLOSE);
                exit(EXIT_FAILURE);
            }
			
			//TODO acho esta função desnecessária, podia estar como no client
            nread = read_bytes_tcp(newfd, request, strlen(request));
            if(nread == ERROR) {
                fprintf(stderr, ERROR_READ);
                exit(EXIT_FAILURE);
            }
			
			//make request a string
			request[nread] = '\0'

            // process request buffer and handle to corresponding functions
			tcp_select_requests_handler(request, nread, reply);

			//TODO write reply back to client
			//TODO close socket and kill child process
			
        } 

    }

}


// set up TCP SOCKET
void tcp_setup(socket_ds* sockets_ds, input_args args) {


    int ret;

    if((sockets_ds->fd_tcp = socket(AF_INET, SOCK_STREAM, AUTO_PROTOCOL)) == ERROR) {
    
        fprintf(stderr, ERROR_FD_TCP);
        exit(EXIT_FAILURE);

    } 

    memset(&sockets_ds->addrinfo_tcp, 0, sizeof(sockets_ds->addrinfo_tcp));
    sockets_ds->addrinfo_tcp.ai_family = AF_INET; //IPv4
    sockets_ds->addrinfo_tcp.ai_socktype = SOCK_DGRAM;  //TCP socket
    sockets_ds->addrinfo_tcp.ai_flags = AI_PASSIVE; 

    if((ret = getaddrinfo(NULL,args.port,&sockets_ds->addrinfo_tcp, &sockets_ds->addrinfo_tcp_ptr) != SUCCESS)) {
        fprintf(stderr, ERROR_ADDR_TCP);
		exit(EXIT_FAILURE);

    }

    if(bind(sockets_ds->fd_tcp,sockets_ds->addrinfo_tcp_ptr->ai_addr,sockets_ds->addrinfo_tcp_ptr->ai_addrlen) == ERROR) {
        fprintf(stderr, ERROR_BIND_TCP);
		exit(EXIT_FAILURE);
    }

    if(listen(sockets_ds->fd_tcp,MAX_QUEUED_REQUESTS) == ERROR) {
        fprintf(stderr,ERROR_LISTEN);
        exit(EXIT_FAILURE);
    }

}



// frees addrinfo and closes connection (fd)
void cleanup_connection(int fd,struct addrinfo *addr) {

	freeeaddrinfo(addr);
	close(fd);
	
}
