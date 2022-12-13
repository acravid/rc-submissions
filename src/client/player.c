/*
 * File: player.c
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: player application for a remote word game
 * 
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "commands/commands.h"

// prints usage message to stderr
static void usage() {
	fprintf(stderr, USAGE_INFO);
}


// defines the game server IP and its port based on input arguments
optional_args parse_opt(int argc, char **argv) {

    // checks if the program was ran with valid arguments
    if (argc == 2 || argc == 4 || argc > 5) {
	    usage();
	    exit(EXIT_FAILURE);
    }

  	optional_args opt_args = {DEFAULT_GSIP , DEFAULT_GSPORT};
    
    if (argc >= 3) {
    	if (strcmp(argv[1], "-n") == EQUAL) {
    	    opt_args.ip = argv[2];
            if (argc == 5 & strcmp(argv[3], "-p") == EQUAL)
    			opt_args.port = argv[4];
    	    else {
    	        usage();
	        	exit(EXIT_FAILURE);
            }
		}
        else if (strcmp(argv[1], "-p") == EQUAL) {
	    	opt_args.port = argv[2];
            if (argc == 5 & strcmp(argv[3], "-n") == EQUAL) 
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

void print_start(game_status* game_stats) {
	printf("New game started (max %d errors): ", game_stats->errors);
	for (int i = 1; i <= game_stats->letters; i++) {
		printf("_ ");
	}
	printf("\n");
}

void handle_input(socket_ds* sockets_ds, game_status* game_stats) {

    // receives the command from  stdin 
    char command[MAX_STRING]; 

    // reads commands indefinitely until exit is given
    while (PROGRAM_IS_RUNNING) {

		// prompt
		printf("[>]");
		// get command
		get_word(command);

		if (strcmp(command, START_COMMAND) == EQUAL || strcmp(command, SHORT_START_COMMAND) == EQUAL) {
			if (send_start_message(sockets_ds, game_stats) == SUCCESS) {
				print_start(game_stats);
			}
		}
		else if (strcmp(command, PLAY_COMMAND) == EQUAL || strcmp(command, SHORT_PLAY_COMMAND) == EQUAL) {
			send_play_message();
		}
		else if (strcmp(command, GUESS_COMMAND) == EQUAL || strcmp(command, SHORT_GUESS_COMMAND) == EQUAL) {
			send_guess_message(); 
		}
		else if (strcmp(command, SCOREBOARD_COMMAND) == EQUAL || strcmp(command, SHORT_SCOREBOARD_COMMAND) == EQUAL) {
			send_scoreboard_message();
		}
		else if (strcmp(command, HINT_COMMAND) == EQUAL || strcmp(command, SHORT_HINT_COMMAND) == EQUAL) {
			send_hint_message();
		}
		else if (strcmp(command, STATE_COMMAND) == EQUAL || strcmp(command, SHORT_STATE_COMMAND) == EQUAL) {
			send_state_message(); 
		}
		else if (strcmp(command, QUIT_COMMAND) == EQUAL) {
			send_quit_message(); 
		}
		else if (strcmp(command, EXIT_COMMAND) == EQUAL) {
			send_quit_message();
	    	break; 
		}
    }
}

void get_word(char* word) {
	
	int i = 0;
	char c = getchar();

	/*starts reading when it finds a non-whitespace char*/
	for (; c == ' ' || c == '\t'; c = getchar()) {
	}
	for (; c != ' ' && c != '\n' && c != '\t' && c != EOF && i <\
			(MAX_STRING - 1); c = getchar(), i++) {
		word[i] = c;
	}
	word[i] = '\0';
}

//
// Function: main
//
//
// Inputs: int argcm char** argv
//
//
// Description:
//
// Main program entry point
//
int main(int argc, char **argv) {

	optional_args opt_args = parse_opt(argc,argv);

	socket_ds* sockets_ds = (socket_ds*) malloc(sizeof(socket_ds)); //socket_setup	
	udp_setup(sockets_ds, opt_args);
	
	game_status* game_stats = (game_status*) malloc(sizeof(game_status));	
	handle_input(sockets_ds, game_stats);
	freeaddrinfo(sockets_ds->addrinfo_udp_ptr);
	close(sockets_ds->fd_udp);
	free(sockets_ds);
	free(game_stats);
	exit(EXIT_SUCCESS);

}
