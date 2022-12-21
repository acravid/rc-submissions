/*
 * File: player.c
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: player application for a remote word game
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "player.h"
#include "commands/commands.h"

/*---------------Function prototypes---------------*/

/*Input: */
optional_args parse_opt(int, char**);
void get_word(char*);

/*Output: */
static void usage();
void print_start(game_status*);
void print_start(game_status*);
void print_play(game_status*);
void print_guess(game_status*);
void print_scoreboard(game_status*);
void print_hint(game_status*, int);
void print_state(game_status*);

/*Play the game: */
void handle_input(socket_ds*, optional_args, game_status*);
void end_game(game_status*);

/*---------------Functions---------------*/

int main(int argc, char **argv) {
	
	//flags setup
	optional_args opt_args = parse_opt(argc,argv);
	
	//udp socket setup	
	socket_ds* sockets_ds = (socket_ds*) malloc(sizeof(socket_ds));
	udp_setup(sockets_ds, opt_args);
							
	//game setup
	game_status* game_stats = (game_status*) malloc(sizeof(game_status));
	game_stats->running = NO;												
	
	// waits for, reads and handles user inputs
	handle_input(sockets_ds, opt_args, game_stats);

	// Exiting
	freeaddrinfo(sockets_ds->addrinfo_udp_ptr);
	close(sockets_ds->fd_udp);
	free(sockets_ds);
	
	end_game(game_stats);
	free(game_stats);
	
	exit(EXIT_SUCCESS);
}


// Prints usage message to stderr
static void usage() {
	fprintf(stderr, USAGE_INFO);
}


// Defines the game server IP and its port based on input arguments
optional_args parse_opt(int argc, char **argv) {
    // checks if the program was ran with valid arguments
    if(argc == 2 || argc == 4 || argc > 5) {
	    usage();
	    exit(EXIT_FAILURE);
    }
	
	// sets default values for server IP and port
  	optional_args opt_args = {DEFAULT_GSIP, DEFAULT_GSPORT};
    
    // alters server IP and port based on flags
    if(argc >= 3) {
    	if(strcmp(argv[1], "-n") == EQUAL) {
    	    opt_args.ip = argv[2];
            if(argc == 5 & strcmp(argv[3], "-p") == EQUAL)
    			opt_args.port = argv[4];
    	    else {
    	        usage();
	        	exit(EXIT_FAILURE);
            }
		}
        else if(strcmp(argv[1], "-p") == EQUAL) {
	    	opt_args.port = argv[2];
            if(argc == 5 & strcmp(argv[3], "-n") == EQUAL) 
    	        opt_args.ip = argv[4];
	    	else {
	        	usage();
	        	exit(EXIT_FAILURE);
            }
		}
        else {
    	    usage();
            exit(EXIT_FAILURE);
        }
    }
	
    return opt_args;
}


// Reads commands from stdin and calls the apropriate function to handle them
void handle_input(socket_ds* sockets_ds, optional_args opt_args, game_status* game_stats) {
    // receives the command from stdin 
    char command[MAX_STRING]; 

    // reads commands indefinitely until exit is given
    while(PROGRAM_IS_RUNNING) {
		//get command
		get_word(command);
		
		//start command
		if(strcmp(command, START_COMMAND) == EQUAL || strcmp(command, SHORT_START_COMMAND) == EQUAL) {
			if(send_start_request(sockets_ds, game_stats) == SUCCESS)
				print_start(game_stats);
		}
		//play command
		else if(strcmp(command, PLAY_COMMAND) == EQUAL || strcmp(command, SHORT_PLAY_COMMAND) == EQUAL) {
			if(send_play_request(sockets_ds, game_stats) == SUCCESS) {
				print_play(game_stats);
				if(game_stats->last_play == WIN || game_stats->last_play == OVR)
					end_game(game_stats);
			}
		}
		//guess command
		else if(strcmp(command, GUESS_COMMAND) == EQUAL || strcmp(command, SHORT_GUESS_COMMAND) == EQUAL) {
			if(send_guess_request(sockets_ds,game_stats) == SUCCESS) {
				print_guess(game_stats);
				if (game_stats->last_play == WIN || game_stats->last_play == OVR)
					end_game(game_stats);				
			}
			
		}
		//scoreboard command
		else if(strcmp(command, SCOREBOARD_COMMAND) == EQUAL || strcmp(command, SHORT_SCOREBOARD_COMMAND) == EQUAL) {
			if(send_scoreboard_request(sockets_ds, opt_args, game_stats) == SUCCESS)
				print_scoreboard(game_stats);
		}
		//hint command
		else if (strcmp(command, HINT_COMMAND) == EQUAL || strcmp(command, SHORT_HINT_COMMAND) == EQUAL) {
			int hint_filesize = send_hint_request(sockets_ds, opt_args,  game_stats);
			if (hint_filesize != ERROR)
				print_hint(game_stats, hint_filesize);
		}
		//state command
		else if (strcmp(command, STATE_COMMAND) == EQUAL || strcmp(command, SHORT_STATE_COMMAND) == EQUAL) {
			if(send_state_request(sockets_ds, opt_args, game_stats) == SUCCESS) {
				print_state(game_stats);
				if(strcmp(game_stats->state_status, "FIN") == EQUAL)
					end_game(game_stats);
			}
		}
		//quit command
		else if (strcmp(command, QUIT_COMMAND) == EQUAL) {
			if (send_quit_request(sockets_ds, game_stats) == SUCCESS) 
				printf(QUIT_MESSAGE);

		}
		//exit command
		else if (strcmp(command, EXIT_COMMAND) == EQUAL) {
			send_quit_request(sockets_ds, game_stats);
	    	break; 
		}
    }
}


//Writes a word from stdin to the variable word
void get_word(char* word) {
	
	int i = 0;
	char c = getchar();

	//starts reading when it finds a non-whitespace char
	for(; c == ' ' || c == '\t'; c = getchar()) {
	}
	//stops reading when it finds a non-whitespace char
	for(; c != ' ' && c != '\n' && c != '\t' && c != EOF && i <\
			(MAX_STRING - 1); c = getchar(), i++) {
		word[i] = c;
	}
	word[i] = '\0';
}


//Prints response to a successful start command 
void print_start(game_status* game_stats) {
	
	printf("New game started (max %d errors): ", game_stats->n_errors);
	for(int i = 1; i <= game_stats->n_letters; i++) {
		printf("_ ");
	}
	printf("\n");

}


//Prints response to a successful play command 
void print_play(game_status* game_stats) {
	//correct play
	if(game_stats->last_play == OK || game_stats->last_play == WIN)
		printf("Yes, \"%c\" is part of the word: ", game_stats->last_letter);
	//wrong play
	else if(game_stats->last_play == NOK || game_stats->last_play == OVR)
		printf("No, \"%c\" is not part of the word: ", game_stats->last_letter);
	//duplicate play
	else if(game_stats->last_play == DUP)
		printf("You already guessed \"%c\" as part of the word: ", game_stats->last_letter);
	//prints word
	for(int i = 0; i <= game_stats->n_letters - 1; i++) {
		printf("%c ", game_stats->word[i]);
	}
	printf("\n");
	//won the game
	if(game_stats->last_play == WIN) 
		printf("WELL DONE! You guessed the word!\n");
	//lost the game
	else if(game_stats->last_play == OVR) 
		printf("You have no more attempts. You lost.\n");

}


//Prints response to a successful guess command 
void print_guess(game_status* game_stats) {
	//correct guess
	if(game_stats->last_play == WIN) {
		int len = strlen(game_stats->guess);
		printf("WELL DONE! You guessed: ");
		for(int i = 0; game_stats->guess[i] != '\0'; i++) {
			printf(i == len -1?"%c\n":"%c ", toupper(game_stats->guess[i]));
		}
	}
	//wrong guess
	if(game_stats->last_play == NOK) {
		printf("Wrong guess.\n");
	}
	//lost the game
	else if(game_stats->last_play == OVR) {
		printf("Wrong guess. No attempts left. You lost.\n");
	}
}


//Prints response to a successful scoreboard command 
void print_scoreboard(game_status* game_stats) {
	
	FILE* file = fopen(game_stats->scoreboard_filename, "r");
	int c;
	while ((c = fgetc(file)) != EOF)
		printf("%c", c);
	fclose(file);

}


//Prints response to a successful hint command 
void print_hint(game_status* game_stats, int size) {
	printf("Received file %s (%d Bytes) with a hint.\n", game_stats->hint_filename, size);
}


//Prints response to a successful state command 
void print_state(game_status* game_stats) {

	FILE* file = fopen(game_stats->state_filename, "r");
	int c;
	while ((c = fgetc(file)) != EOF)
		printf("%c", c);
	fclose(file);

}

//Alters game_stats to reflect the end of a game
void end_game(game_status* game_stats) {
	
	if (game_stats->running == YES) 
		free(game_stats->word);

}
