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
#include "../common/common.h"


buffer_typedef(char,byte_buffer);

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

socket_ds  *setup_socket(optional_args opt_args) {


	// TODO
	// add signal handling 

	socket_ds sockets_ds =  { 0, 0, NULL, NULL};
	socket_ds *socket_ds_ptr = &sockets_ds;
	
	udp_setup(&sockets_ds,opt_args);

	//  TCP connection is only started when a certain command is called
	//  no need to start it at the onset
	// tcp_setup(&sockets_ds,opt_args);

	return socket_ds_ptr;



}

void handle_input(socket_ds *socket_ds_ptr) {



    // receives the input
	byte_buffer input_line;
	buffer_init(input_line,BUFFER_VALUE,char);

	// same as     char input_line[BUFFER_VALUE];

    // receives the command from  stdin 
    char* command; 

    // reads commands indefinitely until exit is given
    while (PROGRAM_IS_RUNNING) {


		// prompt
		printf("[>]");
		// get command
		fgets(input_line.info, input_line.size, stdin);
		// read up to the string terminator character
		input_line.info[input_line.size -1 ] = '\0';


		// separates the command from the rest of the input
		command = strtok(input_line.info, " ");

		if (strcmp(command, START_COMMAND) == EQUAL || strcmp(command, SHORT_START_COMMAND) == EQUAL) {
			send_start_message();
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


//
// Function: main
//
//
// Inputs: int argcm char** argv
//
//
// Description:
//
//
// Main program entry point
//
int main(int argc, char **argv) {

	optional_args opt_args = parse_opt(argc,argv);
	socket_ds *socket_ds_ptr = setup_socket(opt_args);
	handle_input(socket_ds_ptr);
	exit(EXIT_SUCCESS);

}
