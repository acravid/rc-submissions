#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "../GS.h"
#include "request.h"

player_info games[PLAYERID_MAX - PLAYERID_MIN];
bool verbose;
FILE* word_file;

//--------------------------------------------------------------
//                  UDP Module                                  
//--------------------------------------------------------------
void init_player_info(input_args args) {
	verbose = args.verbose_flag;
	word_file = fopen(args.word_file, "r");
	
	for (int i = 0; i < PLAYERID_MAX - PLAYERID_MIN; i++) {
		games[i].trial = 0;
		strcpy(games[i].word, "");
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

void start_game(int playerid) {
	//TODO sizes como deve ser
	char word[MAX_WORD_SIZE];
	char hint[10000];
	char line[1000];

	fgets(line, 1000, word_file);
	sscanf(line, "%s %s", word, hint);
		
	int len = strlen(word);
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

void start_request_handler(char *buffer, size_t len, char *reply) {
	printf("a\n");
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s\n", ERROR_REPLY_CODE);
	else {
		printf("b\n");
		//read player id
		char playerid[PLAYERID_SIZE + 1];

		sscanf(&buffer[CODE_SIZE], "%s", playerid);
		printf("%s\n%s", buffer, playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[len - 1] != '\n') {
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);
			printf("c\n");
		}
		//check if player is valid
		else if (!valid_playerid(playerid)) {
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);
			printf("d\n");
		}
		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN].trial > 1) {
			sprintf(reply,"%s %s\n", START_REPLY_CODE, NOK_REPLY_CODE);
			printf("e\n");
		}

		//starts the game
		else {
			start_game(atoi(playerid));
			sprintf(reply,"%s %s %d %d\n", START_REPLY_CODE, OK_REPLY_CODE, 
					games[atoi(playerid) - PLAYERID_MIN].n_letters,
					games[atoi(playerid) - PLAYERID_MIN].n_errors);
		}
	}
}

void play(int playerid, char letter, char* res) {

	games[playerid - PLAYERID_MIN].trial += 1;
	printf("word: %s \nletter: %c\n", games[playerid - PLAYERID_MIN].word, letter);

	if (strchr(games[playerid - PLAYERID_MIN].word, letter) == NULL) {
		games[playerid - PLAYERID_MIN].n_errors -= 1;
		int len = strlen(games[playerid - PLAYERID_MIN].played_letters);
		games[playerid - PLAYERID_MIN].played_letters[len] = letter;
		games[playerid - PLAYERID_MIN].played_letters[len + 1] = '\0';
		if (games[playerid - PLAYERID_MIN].n_errors < 0) {
			sprintf(res, "%s %d", OVR_REPLY_CODE, games[playerid - PLAYERID_MIN].trial - 1);
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;
		}
		else
			sprintf(res, "%s %d", NOK_REPLY_CODE, games[playerid - PLAYERID_MIN].trial - 1);
	}
	else {
		int count = 0;
		int pos[MAX_WORD_SIZE];
		for (int i = 0; i < (int) strlen(games[playerid - PLAYERID_MIN].word); i++) {
			if (games[playerid - PLAYERID_MIN].word[i] == letter) {
				pos[count] = i + 1;
				count += 1;
			}
		}
		games[playerid - PLAYERID_MIN].n_letters -= count;
		int len = strlen(games[playerid - PLAYERID_MIN].played_letters);
		games[playerid - PLAYERID_MIN].played_letters[len] = letter;
		games[playerid - PLAYERID_MIN].played_letters[len + 1] = '\0';
		if (games[playerid - PLAYERID_MIN].n_letters == 0) {
			sprintf(res, "%s %d", WIN_REPLY_CODE, games[playerid - PLAYERID_MIN].trial - 1);
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;
		}
		else {
			sprintf(res, "%s %d %d", OK_REPLY_CODE, games[playerid - PLAYERID_MIN].trial - 1, count);
			for (int i = 0; i < count; i++) {
				sprintf(&res[strlen(res)], " %d", pos[i]);
			}
			printf("%s\n", res);
		}
	}
}

void play_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	size_t request_size = CODE_SIZE + 1 + PLAYERID_SIZE + 5;
	if (request_size != len && request_size + 1 != len) //trial may have 2 digits
		sprintf(reply, "%s\n", ERROR_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		char letter[2];
		char trial[3];
		sscanf(&buffer[CODE_SIZE], "%s %s %s", playerid, letter, trial);

		//check if parsing was successful
		if (playerid == NULL || letter == NULL || trial == NULL) 
			sprintf(reply, "%s\n", ERROR_REPLY_CODE);

		//check for spaces in the right places
		else if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != ' ' || 
			buffer[CODE_SIZE + 1 + PLAYERID_SIZE + 1 + 1] != ' ') 
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);
	
		//check if the message ends with \n
		else if (buffer[len - 1] != '\n')
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);
	
		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);	

		//check if letter is valid
		else if (!valid_letter(letter[0]))
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);

		//check if letter is duplicate
		else if (strchr(games[atoi(playerid) - PLAYERID_MIN].played_letters, tolower(letter[0])) != NULL)
			sprintf(reply,"%s %s %s\n", PLAY_REPLY_CODE, DUP_REPLY_CODE, trial);
	
		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN].trial < 1)
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, ERROR_REPLY_CODE);

		//check if trial is valid
		else if (atoi(trial) != games[atoi(playerid) - PLAYERID_MIN].trial)
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, INV_REPLY_CODE);

		//make play
		else {
	   		char res[MAX_PLAY_REPLY_SIZE];	
			play(atoi(playerid), tolower(letter[0]), res);
			sprintf(reply,"%s %s\n", PLAY_REPLY_CODE, res);
		}
	}
}

void guess(int playerid, char* word, char* res) {

	games[playerid - PLAYERID_MIN].trial += 1;

	if (strcmp(games[playerid - PLAYERID_MIN].word, word) != EQUAL) {
		games[playerid - PLAYERID_MIN].n_errors -= 1;
		if (games[playerid - PLAYERID_MIN].n_errors < 0) {
			sprintf(res, "%s", OVR_REPLY_CODE);
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;
		}
		else
			sprintf(res, "%s", NOK_REPLY_CODE);
	}
	else {
		printf("Ganhou\n");
		sprintf(res, "%s", WIN_REPLY_CODE);
		games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
		games[playerid - PLAYERID_MIN].trial = 0;
	}
}


void guess_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	size_t request_size = CODE_SIZE + 1 + PLAYERID_SIZE + 5; //not knowing the word size 1 is min
	if (request_size > len && request_size + 1 > len) //trial may have 2 digits
		sprintf(reply, "%s\n", ERROR_REPLY_CODE);
	else {
		printf("aqui\n");
		//read player id, word and trial
		char playerid[PLAYERID_SIZE + 1];
		char word[MAX_WORD_SIZE + 1];
		char trial[3];

		sscanf(&buffer[CODE_SIZE], "%s %s %s", playerid, word, trial);
		printf("%s\n", playerid);
		//check if parsing was successful
		if(playerid == NULL || word == NULL || trial == NULL)
			sprintf(reply, "%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);

		//check for spaces in the right places
		else if(buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != ' ') {
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);
			printf("a\n");
		}
		//check if the message ends with \n
		else if(buffer[len - 1] != '\n')  {
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);
			printf("b\n");
		}
		//check if player is valid
		else if(!valid_playerid(playerid)){
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);
			printf("c\n");
		}

		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN].trial < 1){
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);
			printf("d\n");
		}

		//check if trial is valid
		else if (atoi(trial) != games[atoi(playerid) - PLAYERID_MIN].trial) {
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, INV_REPLY_CODE);
			printf("e\n");
		}

		//make play
		else {
	   		char res[MAX_GUESS_REPLY_SIZE];
	   		if (!word_tolower(word))
	   			sprintf(reply,"%s\n", ERROR_REPLY_CODE);
	   		else {
				guess(atoi(playerid), word, res);
				sprintf(reply,"%s %s %s\n", GUESS_REPLY_CODE, res, trial);
			}
		}
	}	
}


void quit_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len)
		sprintf(reply,"%s\n", ERROR_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		sscanf(&buffer[CODE_SIZE], "%s", playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n')
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);

		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);
		//ends the game
		else {
			games[atoi(playerid) - PLAYERID_MIN].played_letters[0] = '\0';
			games[atoi(playerid) - PLAYERID_MIN].trial = 0;
			sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, OK_REPLY_CODE);
		}
	}
}


int udp_select_requests_handler(char *buffer, size_t len, char *reply) {
	printf("%s\n", buffer);
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
    printf("%s\n", reply);
}


// handle requests to separate handlers
void udp_request_handler(socket_ds* sockets_ds) {

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
        nread = recvfrom(sockets_ds->fd_udp,buffer,CLIENT_UDP_MAX_REQUEST_SIZE,AUTO_PROTOCOL,(struct sockaddr*)&addr, &addrlen);
        if(nread == ERROR) {
            cleanup_connection(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr);
            fprintf(stderr,ERROR_RECV_FROM);
            exit(EXIT_FAILURE);
        }

  
        // turn request to string 
        buffer[nread] = '\0';
        udp_select_requests_handler(buffer, nread, reply);
      

        // send reply back to client
        n = sendto(sockets_ds->fd_udp, reply, strlen(reply), AUTO_PROTOCOL, (struct sockaddr*)&addr,addrlen);
        if(n == ERROR) {
            cleanup_connection(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr);
            fprintf(stderr,ERROR_SENDO_TO);
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

void scoreboard(char* reply) {
	char scoreboard_path_name[strlen(SCOREBOARD_FILE_NAME) + strlen(SCOREBOARD_FILE_PATH)];
	char data[MAX_FILE_SIZE];
	int size = 0;
	sprintf(scoreboard_path_name, "%s%s", SCOREBOARD_FILE_PATH, SCOREBOARD_FILE_NAME);
	FILE* scoreboard_file = fopen(SCOREBOARD_FILE_NAME, "r");
	
	for (char c = fgetc(scoreboard_file); c != EOF; c = fgetc(scoreboard_file), size += 1)
		data[size] = c;
	
	if (size == 0)
		sprintf(reply, "%s %s\n", SCOREBOARD_REPLY_CODE, EMPTY_REPLY_CODE);
	else
		sprintf(reply, "%s %s %s %d %s", SCOREBOARD_REPLY_CODE, OK_REPLY_CODE, SCOREBOARD_FILE_NAME, size, data);
		
	fclose(scoreboard_file);
}

void scoreboard_request_handler(char* request, size_t len, char* reply) {
	//check if request is correct
	if (strcmp("GSB\n", request) != EQUAL)
		sprintf(reply, "%s %s\n", SCOREBOARD_REPLY_CODE, ERROR_REPLY_CODE);
	else {
		// TODO calculate_scoreboard penso que nao precisa de receber nem devolver nada
		// TODO ter uma função que mete o file da scoreboard vazio quando o server morre
		// se deres mesmo delete no file o calculate_scoreboard tem que criar o file porque eu assumo
		//que ele existe na função scoreboard
		//calculate_scoreboard();
		scoreboard(reply);
	}
}

void hint(char* hint_name, char* reply) {
	
	char hint_path_name[strlen(hint_name) + strlen(HINT_FILE_PATH)];
	char data[MAX_FILE_SIZE];
	int size = 0;

	sprintf(hint_path_name, "%s%s", HINT_FILE_PATH, hint_name);
	FILE* hint_file = fopen(hint_path_name, "r");
	
	for (char c = fgetc(hint_file); c != EOF; c = fgetc(hint_file), size += 1)
		data[size] = c;
	
	sprintf(reply, "%s %d %s", hint_name, size, data);
	fclose(hint_file);
}


void hint_request_handler(char* buffer, size_t len, char* reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s\n", ERROR_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		sscanf(&buffer[CODE_SIZE], "%s", playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n') 
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);

		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s\n", ERROR_REPLY_CODE);

		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN].trial < 1) 
			sprintf(reply,"%s %s\n", HINT_REPLY_CODE, NOK_REPLY_CODE);

		else {
			char hint_name[MAX_FILE_NAME];
			//TODO: get_hint vai ao ficheiro do playerid buscar o nome do hint file e mete em hint_name
			//      se esse ficheiro nao existir o player nao tem jogo e por isso hint_name fica a NULL
			//get_hint(atoi(playerid), hint_name);
			if (hint == NULL)
				sprintf(reply,"%s %s\n", HINT_REPLY_CODE, NOK_REPLY_CODE);
			else {
				char res[MAX_HINT_REPLY_SIZE];
				hint(hint_name, res);
				sprintf(reply, "%s %s %s\n", HINT_REPLY_CODE, OK_REPLY_CODE, res);
			}
		}
	}
}

void state(char* state_name, char* reply) {
	char state_path_name[strlen(state_name) + strlen(STATE_FILE_PATH)];
	char data[MAX_FILE_SIZE];
	int size = 0;
	
	//TODO definir STATE_FILE_PATH
	sprintf(state_path_name, "%s%s", STATE_FILE_PATH, state_name);
	FILE* state_file = fopen(state_path_name, "r");
	
	for (char c = fgetc(state_file); c != EOF; c = fgetc(state_file), size += 1)
		data[size] = c;
	
	sprintf(reply, "%s %d %s", state_name, size, data);
	fclose(state_file);
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

		else {
			char state_file_name[MAX_FILE_NAME];
			char reply[MAX_STATE_REPLY_SIZE];
			//TODO get_state(int, char*) mete em char* o file name do state do player
			// se não existir nem ativo nem no historico mete NULL
			//get_state(atoi(playerid), state_file_name);

			if (state_file_name == NULL) 
				sprintf(reply,"%s %s\n", STATE_REPLY_CODE, NOK_REPLY_CODE);
			else {
				//open and read file
				state(state_file_name, reply);

				sprintf(reply, "%s %s %s\n", HINT_REPLY_CODE, OK_REPLY_CODE, reply);
			}
		}
	}

}

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

// handle requests to different functions
void tcp_request_handler(socket_ds* sockets_ds) {

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);;
    size_t n, w_buffer;
    char request[CLIENT_TCP_MAX_REQUEST_SIZE];
    char reply[SERVER_TCP_MAX_REPLY_SIZE];
    pid_t pid;
    
    while(true) {
		int newfd = ERROR;
		
        do newfd = accept(sockets_ds->fd_tcp, (struct sockaddr*)&addr, &addrlen);
        while((newfd == ERROR) && (errno = EINTR));

        if(newfd == ERROR) {
            cleanup_connection(sockets_ds->fd_tcp,sockets_ds->addrinfo_udp_ptr);
            fprintf(stderr,ERROR_SENDO_TO);
            exit(EXIT_FAILURE);
        }
        // create a new child process for each new connection
        pid = fork();
        if (pid == ERROR) {
            fprintf(stderr,ERROR_FORK);
            exit(EXIT_FAILURE);
        } 
        else if (pid == FORK_CHILD) { // child process
            if (close(sockets_ds->fd_tcp) == ERROR) {
                fprintf(stderr, ERROR_CLOSE);
                exit(EXIT_FAILURE);
            }
			printf("Cria child\n");
			n = read(newfd, request, HINT_REQUEST_SIZE);
            if((int) n == ERROR) {
                fprintf(stderr, ERROR_READ);
                close(newfd);
				exit(EXIT_FAILURE);	
            }
            
			//make request a string
			request[n] = '\0';
			printf("Le: %s", request);
            // process request buffer and handle to corresponding functions
			tcp_select_requests_handler(request, n, reply);
			printf("Tem a resposta: %s\n", reply);
			size_t len = strlen(reply);
			w_buffer = 0;
			//read status filename filesize
			while (w_buffer != len) {
				n = write(newfd, reply, len - w_buffer);
				if ((int) n == ERROR) {
					fprintf(stderr, ERROR_WRITE);
                	close(newfd);
					exit(EXIT_FAILURE);
				}
				w_buffer += n;
			}
			printf("Acaba\n");
			close(newfd);
			
			exit(EXIT_SUCCESS);	
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

	freeaddrinfo(addr);
	close(fd);
	
}
