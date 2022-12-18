#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "request.h"
#include "../game_server.h"



//--------------------------------------------------------------
//                  UDP Module                                  
//--------------------------------------------------------------
bool valid_playerid(char* player_id) {
	int id = atoi(player_id);
	if (id >= PLAYERID_MIN and id <= PLAYERID_MAX)
		return true;
	return false;
}

bool valid_letter(char* letter) {
	char l = letter[0];
	//char in between A and z
	if (l >= 65 && l <= 122)
		return true;
	return false;
}

void start_request_handler(char *buffer, size_t len, char *reply_ptr) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);
	
	//read player id
	char playerid[PLAYERID_SIZE + 1];
	sscanf(&buffer[CODE_SIZE], "%s", playerid);

	//check for space after code and if message ends with \n
	if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n') 
		sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);

	//check if player is valid
	if (!valid_playerid(playerid))
		sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);

	//check if player has an ongoing game
	//TODO omgoing_game_with_moves()
	if (ongoing_game_with_moves(playerid))
		sprintf(reply,"%s %s\n", START_REPLY_CODE, NOK_REPLY_CODE);

	//starts the game
	//TODO start_game()
	else {
	   	char res[6];	
		start_game(playerid, res);
		char n_letters[3];
		char n_errors[2];
		sscanf(res, "%s %s", n_letters, n_errors);
		sprintf(reply,"%s %s %s %s\n", START_REPLY_CODE, OK_REPLY_CODE, n_letters, n_errors);
	}
	
}


void play_request_handler(char *buffer,size_t len,char *reply_ptr) {
	//check if message sent has the right size
	int request_size = CODE_SIZE + 1 + PLAYERID_SIZE + 5;
	if (request_size != len && request_size + 1 != len) //trial may have 2 digits
		sprintf(reply, "%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);

	//read player id
	char playerid[PLAYERID_SIZE + 1];
	char letter[2];
	char trial[3];
	sscanf(&buffer[CODE_SIZE], "%s %s %s", playerid, letter, trial);

	//check if parsing was successful
	if (playerid == NULL || letter == NULL || trial == NULL) 
		sprintf(reply, "%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);

	//check for spaces in the right places
	if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != ' ' || 
			buffer[CODE_SIZE + 1 + PLAYERID_SIZE + 1 + 1] != ' ') 
		sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);
	
	//check if the message ends with \n
	if (buffer[len - 1] != '\n')
		sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);
	
	//check if player is valid
	if (!valid_playerid(playerid))
		sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);	

	//check if letter is valid
	if (!valid_letter(letter))
		sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);

	//check if letter is duplicate
	//TODO dup_letter(char*, char*)
	if (!dup_letter(letter, playerid))
		sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, DUP_REPLY_CODE);
	
	//check if player has an ongoing game
	//TODO on_going_game_w_or_wo_moves
	if (!ongoing_game_w_or_wo_moves(playerid))
		sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);

	//check if trial is valid
	//TODO valid_trial()
	if (!valid_trial(trial, playerid))
		sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, INV_REPLY_CODE);

	//make play
	//TODO play()
	else {
	   	char res[MAX_PLAY_REPLY_SIZE];	
		play(playerid, letter, res);
		sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, res);
	}
}


void guess_request_handler(char *buffer,size_t len,char *reply_ptr) {
	//check if message sent has the right size
	int request_size = CODE_SIZE + 1 + PLAYERID_SIZE + 5; //not knowing the word size 1 is min
	if (request_size > len && request_size + 1 > len) //trial may have 2 digits
		sprintf(reply, "%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);

	//read player id, word and trial
	char playerid[PLAYERID_SIZE + 1];
	char word[MAX_WORD_SIZE + 1];
	char trial[3];
	sscanf(&buffer[CODE_SIZE], "%s %s %s", playerid, letter, trial);

	//check if parsing was successful
	if (playerid == NULL || letter == NULL || trial == NULL)
		sprintf(reply, "%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);

	//check for spaces in the right places
	if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != ' ')
		sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
	
	//check if the message ends with \n
	if (buffer[len - 1] != '\n') 
		sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
	
	//check if player is valid
	if (!valid_playerid(playerid))
		sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);	

	//check if player has an ongoing game
	//TODO on_going_game_w_or_wo_moves
	if (!ongoing_game_w_or_wo_moves(playerid))
		sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);

	//check if trial is valid
	//TODO valid_trial()
	if (!valid_trial(trial, playerid)) 
		sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, INV_REPLY_CODE);

	//make play
	//TODO guess()
	else {
	   	char res[MAX_GUESS_REPLY_SIZE];	
		guess(playerid, word, res);
		sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, res);
	}
}


void quit_request_handler(char *buffer,size_t len,char *reply_ptr) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len)
		sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, ERROR_REPLY_CODE);

	//read player id
	char playerid[PLAYERID_SIZE + 1];
	sscanf(&buffer[CODE_SIZE], "%s", playerid);

	//check for space after code and if message ends with \n
	if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n')
		sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);

	//check if player is valid
	if (!valid_playerid(playerid))
		sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);

	//check if player has an ongoing game
	//TODO omgoing_game_with_moves()
	if (!ongoing_game_with_moves(playerid))
		sprintf(reply,"%s %s\n", START_REPLY_CODE, NOK_REPLY_CODE);

	//starts the game
	//TODO end_game()
	else {
		end_game(playerid);
		sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, OK_REPLY_CODE);
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
    char reply[SERVER_UDP_MAX_REPLY_SIZE];
 
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
            strcpy(reply, CODE_ERROR_CODE);

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


void scoreboard_request_handler(char *buffer,size_t len,char *reply_ptr) {


}


void hint_request_handler(char *buffer,size_t len,char *reply_ptr) {


}


void state_request_handler(char *buffer,size_t len,char *reply_ptr) {


}

void tcp_select_requests_handler(char *buffer,size_t len,char *reply_ptr) {

    if(strncmp(SCOREBOARD_CODE,buffer,GAME_PLAY_CODE_SIZE)) {
        scoreboard_request_handler(buffer,len,reply_ptr);
    } 
    else if(strncmp(HINT_CODE,buffer,GAME_PLAY_CODE_SIZE)) {
        hint_request_handler(buffer,len,reply_ptr);
    }
    else if(strncmp(STATE_CODE,buffer,GAME_PLAY_CODE_SIZE)) {
        status_request_handler(buffer,len,reply_ptr);
    }
     else {
        strcpy(reply_ptr,UNKNOWN_GAME_PLAY_CODE_REPLY);

    }

}


ssize_t read_bytes_tcp(int fd,char *buffer,size_t number_of_bytes) {    

    // FIX ME 
    // add timeout (timer)

    ssize_t read_bytes = 0;
    int reading = 1;

    while(reading) {
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
    char request_buffer[CLIENT_TCP_MAX_REQUEST_SIZE];
    char reply_buffer[SERVER_TCP_MAX_REPLY_SIZE];
    pid_t pid;
 
    memset(reply_buffer,'\0',sizeof(reply_buffer));
    
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

            nread = read_bytes_tcp(newfd,request_buffer,strlen(request_buffer));
            if(nread == ERROR) {
                fprintf(stderr,ERROR_READ);
                exit(EXIT_FAILURE);
            }

            // process request buffer and handle to corresponding functions


        } 
        else {
            // TODO:
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
