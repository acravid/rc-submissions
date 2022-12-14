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

void print_play(game_status* game_stats) {
	if (game_stats->last_play == OK || game_stats->last_play == WIN)
		printf("Yes, \"%c\" is part of the word: ", game_stats->last_letter);
	else if (game_stats->last_play == NOK || game_stats->last_play == OVR)
		printf("No, \"%c\" is not part of the word: ", game_stats->last_letter);
	else if (game_stats->last_play == DUP)
		printf("You already guessed \"%c\" as part of the word: ", game_stats->last_letter);
	for (int i = 0; i <= game_stats->letters - 1; i++) {
		printf("%c ", game_stats->word[i]);
	}
	printf("\n");
	if (game_stats->last_play == WIN) 
		printf("WELL DONE! You guessed the word!\n");
	else if (game_stats->last_play == OVR) 
		printf("You have no more attempts. You lost.\n");
}


void end_game(game_status* game_stats) {
	if (game_stats->running == YES) {
		free(game_stats->word);
		game_stats->running = NO;
	}
}


void force_quit(socket_ds* sockets_ds, game_status* game_stats) {
	char res;
	printf("Do you want to force quit? [y/n]\n");
	scanf("%c", &res);
	if (res != 'n' && res != 'N') {
		if (send_quit_request(sockets_ds, game_stats) == SUCCESS) {
			printf("Game quitted.\n");
		}
	}
}


void print_scoreboard(game_status* game_stats) {
	FILE* file = fopen(game_stats->scoreboard_filename, "r");
	int c;
	while ((c = fgetc(file)) != EOF)
		printf("%c", c);
	fclose(file);
}


void handle_input(socket_ds* sockets_ds, optional_args opt_args, game_status* game_stats) {

    // receives the command from  stdin 
    char command[MAX_STRING]; 

    // reads commands indefinitely until exit is given
    while (PROGRAM_IS_RUNNING) {

		// get command
		get_word(command);

		if (strcmp(command, START_COMMAND) == EQUAL || strcmp(command, SHORT_START_COMMAND) == EQUAL) {
			if (send_start_request(sockets_ds, game_stats) == SUCCESS)
				print_start(game_stats);
			else {
				force_quit(sockets_ds, game_stats);
			}
		}
		else if (strcmp(command, PLAY_COMMAND) == EQUAL || strcmp(command, SHORT_PLAY_COMMAND) == EQUAL) {
			if (send_play_request(sockets_ds, game_stats) == SUCCESS) {
				print_play(game_stats);
				if (game_stats->last_play == WIN || game_stats->last_play == OVR)
					end_game(game_stats);
			}
		}
		else if (strcmp(command, GUESS_COMMAND) == EQUAL || strcmp(command, SHORT_GUESS_COMMAND) == EQUAL) {
			send_guess_message(); 
		}
		else if (strcmp(command, SCOREBOARD_COMMAND) == EQUAL || strcmp(command, SHORT_SCOREBOARD_COMMAND) == EQUAL) {
			if (send_scoreboard_request(sockets_ds, opt_args, game_stats) == SUCCESS)
				print_scoreboard(game_stats);
		}
		else if (strcmp(command, HINT_COMMAND) == EQUAL || strcmp(command, SHORT_HINT_COMMAND) == EQUAL) {
			send_hint_message();
		}
		else if (strcmp(command, STATE_COMMAND) == EQUAL || strcmp(command, SHORT_STATE_COMMAND) == EQUAL) {
			send_state_message(); 
		}
		else if (strcmp(command, QUIT_COMMAND) == EQUAL) {
			if (send_quit_request(sockets_ds, game_stats) == SUCCESS) {
				printf("Game quitted\n");
				end_game(game_stats);
			}
		}
		else if (strcmp(command, EXIT_COMMAND) == EQUAL) {
			if (game_stats->running == YES &&send_quit_request(sockets_ds, game_stats) != SUCCESS)
				printf("Error. Couldn't quit the game in the server.\n");
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
	game_stats->running = NO;
	//game_stats->trial = 14;
	//game_stats->word = (char*) malloc(sizeof(char) * 30);
	//strcpy(game_stats->player_id, "099246");
	handle_input(sockets_ds, opt_args, game_stats);

	// Exiting
	freeaddrinfo(sockets_ds->addrinfo_udp_ptr);
	close(sockets_ds->fd_udp);
	free(sockets_ds);
	end_game(game_stats);
	free(game_stats);
	exit(EXIT_SUCCESS);

}
