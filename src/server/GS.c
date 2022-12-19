#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // for creating directories
#include "GS.h"
#include "requests/request.h"

// create directories that stores game related
// information GAMES and SCORES
void init_data(input_args args) {
	
	init_player_info(args);

	if(mkdir("GAMES",S_IRWXU) != SUCCESS) {
		fprintf(stderr,ERROR_MKDIR);
	}
	
	if(mkdir("SCORES",S_IRWXU) != SUCCESS) {
		fprintf(stderr,ERROR_MKDIR);
	}

}


// prints usage message to stderr
static void usage() {
	fprintf(stderr, USAGE_INFO);
}


// Parses command line of the following format:
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

// clean up server state, that is to say:
// close open socket connections
// free resources
void cleanup_server(socket_ds * sockets_ds) {

	// Exiting
	freeaddrinfo(sockets_ds->addrinfo_udp_ptr);
	close(sockets_ds->fd_udp);
	free(sockets_ds);


}

// Main program entry point
int main(int argc, char **argv) {

	pid_t pid;
	input_args args = parse_args(argc,argv);
	socket_ds* sockets_ds = (socket_ds*) malloc(sizeof(socket_ds)); //socket_setup	


	// initialize data storage
	init_data(args);
	// create a child process
	pid = fork();
	// 0 is returned in the child
	if(pid == 0) {
		// child process responsible for handling udp requests

		// create pointer, useful to pass around functions
		socket_ds* socket_ds_ptr = sockets_ds;
		
		// TODO: 
		// catch ctrl-c with a signal handler
		udp_setup(sockets_ds, args);
		udp_request_handler(sockets_ds);

	} else if(pid > 0) {
		// calling process (parent)
		// responsible for handling tcp requests
		tcp_setup(sockets_ds, args);
		tcp_request_handler(sockets_ds);

	} else {
		// on failure - 1 is returned
		fprintf(stderr,ERROR_FORK);

		// TODO: ?
		// close open tcp and udp connections -> clean up connections
		cleanup_server(sockets_ds);

		exit(EXIT_FAILURE);
	}

	cleanup_server(sockets_ds);
	exit(EXIT_SUCCESS);

}
