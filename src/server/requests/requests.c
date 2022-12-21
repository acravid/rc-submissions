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

//--------------------------------------------------------------
//                  UDP Module                                  
//--------------------------------------------------------------
void init_player_info(input_args args, FILE* file) {
	verbose = args.verbose_flag;
	word_file = file;
	
	for (int i = 0; i < PLAYERID_MAX - PLAYERID_MIN; i++) {
		games[i].trial = 0;
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

void start_game(char *player_id) {

	printf("-a\n");
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

	printf("-a\n");
	games[playerid - PLAYERID_MIN].trial = 1;
	strcpy(games[playerid - PLAYERID_MIN].word, word);
	printf("-a\n");
}

void start_request_handler(char *buffer, size_t len, char *reply) {
	printf("a\n");
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);
	else {
		printf("b\n");
		//read player id
		char playerid[PLAYERID_SIZE + 1];

		sscanf(&buffer[CODE_SIZE], "%s", playerid);
		printf("%s\n%s", buffer, playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[len - 1] != '\n') {
			sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);
			printf("c\n");
		}
		//check if player is valid
		else if (!valid_playerid(playerid)) {
			sprintf(reply,"%s %s\n", START_REPLY_CODE, ERROR_REPLY_CODE);
			printf("d\n");
		}
		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN].trial > 1) {
			sprintf(reply,"%s %s\n", START_REPLY_CODE, NOK_REPLY_CODE);
			printf("e\n");
		}

		//starts the game
		else {
			//checks if last response was lost
			if (strcmp(buffer, games[atoi(playerid) - PLAYERID_MIN].last_request) == EQUAL) {
				strcpy(reply, games[atoi(playerid) - PLAYERID_MIN].last_response);
				printf("was the same\n");
			}
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

void play(char *player_id,char *letter_buffer,char letter, char* res) {
	
	int playerid = atoi(player_id);

	// write play to player's file
	write_game_play_to_file(player_id,letter_buffer,PLAY_CODE);

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

			// NOTE: add comments
			create_player_game_directory(player_id); 
			rename_and_move_player_file(player_id,TERMINATION_STATUS_FAIL);			
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

			// NOTE: add comments
			create_player_game_directory(player_id); 
			rename_and_move_player_file(player_id,TERMINATION_STATUS_WIN);			
			
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

		//make play
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


void guess(char *player_id, char* word, char* res) {

	int playerid = atoi(player_id);

	// write guess to player's file
	write_game_play_to_file(player_id,word,GUESS_CODE);

	games[playerid - PLAYERID_MIN].trial += 1;

	if (strcmp(games[playerid - PLAYERID_MIN].word, word) != EQUAL) {
		games[playerid - PLAYERID_MIN].n_errors -= 1;
		if (games[playerid - PLAYERID_MIN].n_errors < 0) {
			sprintf(res, "%s", OVR_REPLY_CODE);
			games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
			games[playerid - PLAYERID_MIN].trial = 0;
			// NOTE: add comments
			create_player_game_directory(player_id); 
			rename_and_move_player_file(player_id,TERMINATION_STATUS_FAIL);			
		}
		else
			sprintf(res, "%s", NOK_REPLY_CODE);
	}
	else {
		printf("Ganhou\n");
		sprintf(res, "%s", WIN_REPLY_CODE);
		games[playerid - PLAYERID_MIN].played_letters[0] = '\0';
		games[playerid - PLAYERID_MIN].trial = 0;

		// NOTE: add comments
		create_player_game_directory(player_id); 
		rename_and_move_player_file(player_id,TERMINATION_STATUS_WIN);			
			
	}
}

void guess_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	size_t request_size = CODE_SIZE + 1 + PLAYERID_SIZE + 5; //not knowing the word size 1 is min
	if (request_size > len && request_size + 1 > len) //trial may have 2 digits
		sprintf(reply, "%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
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
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
			printf("a\n");
		}
		//check if the message ends with \n
		else if(buffer[len - 1] != '\n')  {
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
			printf("b\n");
		}
		//check if player is valid
		else if(!valid_playerid(playerid)){
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
			printf("c\n");
		}
		
		//check if player has an ongoing game
		else if (games[atoi(playerid) - PLAYERID_MIN].trial < 1){
			sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
			printf("d\n");
		}
		//check if word is valid and makes it lower case
		else if (!word_tolower(word))
	   		sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, ERROR_REPLY_CODE);
		//make guess
		else {
	   		//checks if last response was lost
			if (strcmp(buffer, games[atoi(playerid) - PLAYERID_MIN].last_request) == EQUAL) 
				strcpy(reply, games[atoi(playerid) - PLAYERID_MIN].last_response);
			//check if trial is valid
			else if (atoi(trial) != games[atoi(playerid) - PLAYERID_MIN].trial) {
				sprintf(reply,"%s %s\n", GUESS_REPLY_CODE, INV_REPLY_CODE);
				printf("e\n");
			}
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


void quit_request_handler(char *buffer,size_t len,char *reply) {
	//check if message sent has the right size
	printf("a\n");
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) {
		sprintf(reply,"%s %s\n", QUIT_REPLY_CODE, ERROR_REPLY_CODE);
		printf("b\n");
	}
	else {
		printf("c\n");
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
			printf("aqui nao quero\n");
			
			// NOTE: add comments
			create_player_game_directory(playerid); 
			rename_and_move_player_file(playerid,TERMINATION_STATUS_QUIT);	
				
			
			games[atoi(playerid) - PLAYERID_MIN].played_letters[0] = '\0';
			games[atoi(playerid) - PLAYERID_MIN].trial = 0;
			strcpy(games[atoi(playerid) - PLAYERID_MIN].last_request, "");
			strcpy(games[atoi(playerid) - PLAYERID_MIN].last_response, "");
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
    ssize_t ret_udp_response, ret_udp_request;
    int timeout_count;
 
    memset(reply, '\0', sizeof(reply));
    
    struct sockaddr_in addr;
    socklen_t addrlen;

    while(true) {

        addrlen = sizeof(addr);
		printf("cima\n");
        // receive client message from socket
		ret_udp_request = recvfrom(sockets_ds->fd_udp, buffer, CLIENT_UDP_MAX_REQUEST_SIZE, AUTO_PROTOCOL, (struct sockaddr*) &addr, &addrlen);
		if(ret_udp_request == ERROR) {
			cleanup_connection(sockets_ds->fd_udp,sockets_ds->addrinfo_udp_ptr);
            fprintf(stderr,ERROR_RECV_FROM);
            exit(EXIT_FAILURE);
        }

  		printf("aqui\n");
        // turn request to string
        if (ret_udp_request < CLIENT_UDP_MAX_REQUEST_SIZE)
        	buffer[ret_udp_request] = '\0';
        else
        	buffer[MAX_PLAY_REPLY_SIZE - 1] = '\0';
        
        if (verbose)
			printf("IP: %s Port: %u Sent: %s", inet_ntoa(addr.sin_addr), addr.sin_port, buffer);
			
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
    
    struct timeval timeout;
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    if(setsockopt(sockets_ds->fd_udp, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) != SUCCESS) {
        // Failed to get an internet address
		fprintf(stderr, ERROR_ADDR_UDP);
		exit(EXIT_FAILURE);
    }

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
	FILE* scoreboard_file = fopen(scoreboard_path_name, "r");
	if (scoreboard_file == NULL)
		printf("d\n");
	for (char c = fgetc(scoreboard_file); c != EOF; c = fgetc(scoreboard_file), size += 1)
		data[size] = c;
	
	if (size == 0)
		sprintf(reply, "%s %s\n", SCOREBOARD_REPLY_CODE, EMPTY_REPLY_CODE);
	else
		sprintf(reply, "%s %s %s %d %s", SCOREBOARD_REPLY_CODE, OK_REPLY_CODE, SCOREBOARD_FILE_NAME, size, data);
		
	fclose(scoreboard_file);
	printf("c\n");
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

		else {
			char hint_name[MAX_FILENAME];
			get_hint_filename(hint_name,playerid);
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

void state(char* state_name, char* reply, char* playerid) {
	char state_path_name[strlen(state_name) + strlen(STATE_FILE_PATH)];
	char data[MAX_FILE_SIZE];
	int size = 0;
	
	//TODO definir STATE_FILE_PATH
	sprintf(state_path_name, "%s%s", STATE_FILE_PATH, state_name);
	FILE* state_file = fopen(state_path_name, "r");
	
	for (char c = fgetc(state_file); c != EOF; c = fgetc(state_file), size += 1)
		data[size] = c;
	
	sprintf(reply, "STATE_%s.txt %d %s", playerid, size, data);
	fclose(state_file);
}

void state_request_handler(char* buffer, size_t len, char *reply) {
	//check if message sent has the right size
	if (CODE_SIZE + 1 + PLAYERID_SIZE + 1 != len) 
		sprintf(reply,"%s %s\n", STATE_REPLY_CODE, NOK_REPLY_CODE);
	else {
		//read player id
		char playerid[PLAYERID_SIZE + 1];
		sscanf(&buffer[CODE_SIZE], "%s", playerid);

		//check for space after code and if message ends with \n
		if (buffer[CODE_SIZE] != ' ' || buffer[CODE_SIZE + 1 + PLAYERID_SIZE] != '\n') 
			sprintf(reply,"%s %s\n", STATE_REPLY_CODE, NOK_REPLY_CODE);

		//check if player is valid
		else if (!valid_playerid(playerid))
			sprintf(reply,"%s %s\n", STATE_REPLY_CODE, NOK_REPLY_CODE);

		else {
			char state_file_name[MAX_FILENAME];
			char reply[MAX_STATE_REPLY_SIZE];
			//TODO get_state(int, char*) mete em char* o file name do state do player
			// se não existir nem ativo nem no historico mete NULL
			//get_state(atoi(playerid), state_file_name);

			if (state_file_name == NULL) 
				sprintf(reply,"%s %s\n", STATE_REPLY_CODE, NOK_REPLY_CODE);
			else {
				//open and read file
				state(state_file_name, reply, playerid);

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
    ssize_t ret_udp_request, ret_udp_response;
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
        else if (pid == FORK_CHILD) { // child process
            if (close(sockets_ds->fd_tcp) == ERROR) {
                fprintf(stderr, ERROR_CLOSE);
                exit(EXIT_FAILURE);
            }
			printf("Cria child\n");
            
            ret_udp_request = ERROR;
            timeout_count = 0;
            while (ret_udp_request == ERROR) {
				ret_udp_request = read(newfd, request, CLIENT_TCP_MAX_REQUEST_SIZE);
				if (ret_udp_request == ERROR && timeout_count == MAX_TIMEOUTS) {
					fprintf(stderr, ERROR_READ);
                	close(newfd);
					exit(EXIT_FAILURE);	
				}
				else if (ret_udp_request == ERROR) {
					printf(TIMEOUT_RECV_TCP);
					timeout_count += 1;
				}
			}
            
			//make request a string
			request[ret_udp_request] = '\0';
			printf("Le: %s", request);
            // process request buffer and handle to corresponding functions
			tcp_select_requests_handler(request, ret_udp_request, reply);
			printf("Tem a resposta: %s\n", reply);
			size_t len = strlen(reply);
			ret_udp_request = ERROR;
            timeout_count = 0;
			w_buffer = 0;
			//read status filename filesize
			while (ret_udp_response == ERROR || w_buffer != len) {
				ret_udp_response = write(newfd, reply, len - w_buffer);
				if (ret_udp_response == ERROR && timeout_count == MAX_TIMEOUTS) {
					fprintf(stderr, ERROR_WRITE);
                	close(newfd);
					exit(EXIT_FAILURE);
				}
				else if (ret_udp_response == ERROR) {
					printf(TIMEOUT_SEND_TCP);
					timeout_count += 1;
				}
				else
					w_buffer += ret_udp_response;
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
	
	struct timeval timeout;
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    if(setsockopt(sockets_ds->fd_tcp, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) != SUCCESS) {
        // Failed to get an internet address
		exit(EXIT_FAILURE);
    }

    if(setsockopt(sockets_ds->fd_tcp, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != SUCCESS) {
        // Failed to get an internet address
		exit(EXIT_FAILURE);
    }
    
    if((ret = getaddrinfo(NULL,args.port,&sockets_ds->addrinfo_tcp, &sockets_ds->addrinfo_tcp_ptr) != SUCCESS)) {
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



// frees addrinfo and closes connection (fd)
void cleanup_connection(int fd,struct addrinfo *addr) {

	freeaddrinfo(addr);
	close(fd);
	
}
