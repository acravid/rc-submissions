/*
 * File: GS.c
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: server application for a remote word game
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include "GS.h"
#include "requests/request.h"

/*---------------Function prototypes---------------*/

static void usage();
input_args parse_args(int, char**);
void init_data(input_args);

/*---------------Functions---------------*/

//Handles SIGINT to close the server properly
void handle_signal_action(int sig_number) {
  if (sig_number == SIGINT) {
    printf("\nClosing server\n");
    exit(EXIT_SUCCESS);
  }
}


int main(int argc, char **argv) {

	pid_t pid;
	input_args args = parse_args(argc,argv);
	
	//sets up to catch SIGINT
	signal(SIGINT, &handle_signal_action);
  	
  	socket_ds* sockets_ds = (socket_ds*) malloc(sizeof(socket_ds));
  	
	// initialize data storage
	init_data(args);
	
	// setup sockets
	udp_setup(sockets_ds, args);
	tcp_setup(sockets_ds, args);
	
	init_player_info(args, fopen(args.word_file, "r"));
	
	// create a child process
	pid = fork();
	
	if(pid == 0) {
		//child process responsible for handling udp requests
		udp_request_handler(sockets_ds);

	} else if (pid > 1) {
		//parent process responsible for handling tcp requests
		tcp_request_handler(sockets_ds);

	} else {
		//failure
		printf(ERROR_FORK);

		exit(EXIT_FAILURE);
	}
}


//Prints usage message to stderr
static void usage() {
	fprintf(stderr, USAGE_INFO);
}


//Parses command line of the following format:
// ./GS word_file [-p GSPort] [-v]
input_args parse_args(int argc, char **argv) {

	input_args args;

    if(argc == 1 || argc > 5) {
	    usage();
	    exit(EXIT_FAILURE);
    }

  	args.port = DEFAULT_GSPORT;
  	args.verbose_flag = false;

    if(argc >= 2) {
        
        if(strcmp(argv[1],"-p") == EQUAL|| strcmp(argv[1],"-v") == EQUAL) {
            usage();
            exit(EXIT_FAILURE);
        }

        args.word_file = argv[1];
		for(int i = 2; i < argc; i++) {
			if(strcmp(argv[i],"-p") == EQUAL) {
				args.port = argv[i + 1];
			}
			if(strcmp(argv[i],"-v") == EQUAL) {
				args.verbose_flag = true;
			}
		}                
    }
    
    return args;
}


//Create directories that stores game related information GAMES and SCORES
void init_data(input_args args) {

	if(mkdir(GAME_DATA_DIR,S_IRWXU) == ERROR  && errno != EEXIST) 
		fprintf(stderr, ERROR_MKDIR_GS, strerror(errno));
		
	if(mkdir(GAMES_DIR,S_IRWXU) == ERROR  && errno != EEXIST) 
		fprintf(stderr, ERROR_MKDIR_GS, strerror(errno));
		
	if(mkdir(SCORES_DIR,S_IRWXU) == ERROR  && errno != EEXIST) 
		fprintf(stderr, ERROR_MKDIR_GS, strerror(errno));
}