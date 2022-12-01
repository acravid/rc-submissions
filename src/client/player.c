/*
 * File: player.c
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: player application for a remote word game
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "commands_udp.h"
#include "commands_tcp.h"

void handle_start_error()  {


}

void handle_play_error() {


}


void handle_guess_error() {


} 


void handle_scoreboard_error() {
	 

}


void handle_hint_error() {


}


void handle_state_error() {


}


void handle_quit_error() {


}


void handle_exit_error() {


}


// prints usage message to stderr
static void usage() {
	fprintf(stderr, USAGE_INFO);
}

// defines the game server IP and its port based on input arguments
struct optional_args parse_opt(int argc, char **argv) {

    // checks if the program was ran with valid arguments
    if (argc == 2 || argc == 4 || argc > 5) {
	    usage();
	    exit(EXIT_FAILURE);
    }

    struct optional_args opt_args = {DEFAULT_GSIP , DEFAULT_GSPORT};
    
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


void create_socket_udp_tcp(struct optional_args opt_args) {



	
}

void input_handler() {


    // receives the input
    char input_line[MAX_STRING];
    // receives the command from  stdin 
    char* command; 

    // reads commands indefinitely until exit is given
    while (PROGRAM_IS_RUNNING) {

		// reads from sdin
		fgets(input_line, MAX_STRING, stdin);
		// separates the command from the rest of the input
		command = strtok(input_line, " ");

		if (strcmp(command, START_COMMAND) == EQUAL || strcmp(command, SHORT_START_COMMAND) == EQUAL) {
	    	if (send_start_message() == ERROR)
				handle_start_error(); 
		}
		else if (strcmp(command, PLAY_COMMAND) == EQUAL || strcmp(command, SHORT_PLAY_COMMAND) == EQUAL) {
	    	if (send_play_message() == ERROR)
				handle_play_error(); 
		}
		else if (strcmp(command, GUESS_COMMAND) == EQUAL || strcmp(command, SHORT_GUESS_COMMAND) == EQUAL) {
	    	if (send_guess_message() == ERROR)
				handle_guess_error(); 
		}
		else if (strcmp(command, SCOREBOARD_COMMAND) == EQUAL || strcmp(command, SHORT_SCOREBOARD_COMMAND) == EQUAL) {
	    	if (send_scoreboard_message() == ERROR)
				handle_scoreboard_error(); 
		}
		else if (strcmp(command, HINT_COMMAND) == EQUAL || strcmp(command, SHORT_HINT_COMMAND) == EQUAL) {
	    	if (send_hint_message() == ERROR)
				handle_hint_error(); 
		}
		else if (strcmp(command, STATE_COMMAND) == EQUAL || strcmp(command, SHORT_STATE_COMMAND) == EQUAL) {
	    	if (send_state_message() == ERROR)
				handle_state_error(); 
		}
		else if (strcmp(command, QUIT_COMMAND) == EQUAL) {
	    	if (send_quit_message() == ERROR)
				handle_quit_error(); 
		}
		else if (strcmp(command, EXIT_COMMAND) == EQUAL) {
	    	if (send_quit_message() == ERROR)
				handle_exit_error();
	    	break; 
		}
    }
}


int main(int argc, char **argv) {

	struct optional_args opt_args = parse_opt(argc,argv);
	create_socket_udp_tcp(opt_args);
	//input_handler();
	exit(EXIT_SUCCESS);

}
