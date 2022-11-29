/*
 * File: player.c
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: player application for a remote word game
*/

#include <stdio.h>
#include <stdlib.h>
#include "player.h"



/*---------------Functions---------------*/


struct optional_args parse_opt(int argc, char **argv) {

	struct optional_args opt_args;
	
	opt_args.ip = DEFAULT_GSIP;
	opt_args.port = DEFAULT_GSPORT;
	
	/*checks if the program was ran with valid arguments*/
    if (argc == 2 || argc == 4 || argc > 5) {
		// TODO print to stderr 
		printf("Invalid arguments to start the player\n");
		exit(EXIT_FAILURE);
    }
    else if (argc >= 3) {
    	if (cmp_str(argv[1], "-n") == EQUAL) {
    	    opt_args.ip = argv[2];
            if (argc == 5 & cmp_str(argv[3], "-p") == EQUAL)
    			opt_args.port = atoi(argv[4]);
    	    else {
    	        printf("Invalid arguments to start the player\n");
	        	exit(EXIT_FAILURE);
            }
		}
	}
    else if (cmp_str(argv[1], "-p") == EQUAL) {

		opt_args.port = atoi(argv[2]);
        if (argc == 5 & cmp_str(argv[3], "-n") == EQUAL) 
    		opt_args.ip = argv[4];
		else {
			printf("Invalid arguments to start the player\n");
			exit(EXIT_FAILURE);
		}
	}
    else {
    	printf("Invalid arguments to start the player\n");
	    exit(1);
	}

	return opt_args;
}


void create_socket_udp_tcp() {

	
}

void input_handler() {


	/*receives the input*/
	char input_line[MAX_STRING];
	/*receives the command from  stdin*/ 
	char* command; 

    /*reads commands indefinitely until exit is given*/
    while (PROGRAM_IS_RUNNING) {


		/* reads from sdin*/
		fgets(input_line, MAX_STRING, stdin);
		/*separates the command from the rest of the input*/
		command = strtok(input_line, " ");

		if (cmp_str(command, START_COMMAND) == EQUAL || cmp_str(command, SHORT_START_COMMAND) == EQUAL) {
	    	if (send_start_message(input_line) == ERROR)
			handle_start_error(); 
		}
		else if (cmp_str(command, PLAY_COMMAND) == EQUAL || cmp_str(command, SHORT_PLAY_COMMAND) == EQUAL) {
	    	if (send_play_message(input_line) == ERROR)
			handle_play_error(); 
		}
		else if (cmp_str(command, GUESS_COMMAND) == EQUAL || cmp_str(command, SHORT_GUESS_COMMAND) == EQUAL) {
	    	if (send_guess_message(input_line) == ERROR)
			handle_guess_error(); 
		}
		else if (cmp_str(command, SCOREBOARD_COMMAND) == EQUAL || cmp_str(command, SHORT_SCOREBOARD_COMMAND) == EQUAL) {
	    	if (send_scoreboard_message() == ERROR)
			handle_scoreboard_error(); 
		}
		else if (cmp_str(command, HINT_COMMAND) == EQUAL || cmp_str(command, SHORT_HINT_COMMAND) == EQUAL) {
	    	if (send_hint_message() == ERROR)
			handle_hint_error(); 
		}
		if (cmp_str(command, STATE_COMMAND) == EQUAL || cmp_str(command, SHORT_STATE_COMMAND) == EQUAL) {
	    	if (send_state_message() == ERROR)
			handle_state_error(); 
		}
		if (cmp_str(command, QUIT_COMMAND) == EQUAL) {
	    	if (send_quit_message() == ERROR)
			handle_quit_error(); 
		}
		if (cmp_str(command, EXIT_COMMAND) == EQUAL) {
	    	if (send_quit_message == ERROR)
			handle_exit_error();
	    	break; 
		}

	}

}


int main(int argc, char **argv) {

	parse_opt(argc,argv);
	create_socket_udp_tcp();
	input_handler();
	exit(EXIT_SUCCESS);

}
