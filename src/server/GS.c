#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // for creating directories
#include <signal.h>
#include "GS.h"
#include "requests/request.h"

socket_ds* sockets_ds;
FILE* file;

// clean up server state, that is to say:
// close open socket connections
// free resources
void cleanup_server() {
	// Exiting
	fclose(file);
	freeaddrinfo(sockets_ds->addrinfo_udp_ptr);
	close(sockets_ds->fd_udp);
	freeaddrinfo(sockets_ds->addrinfo_tcp_ptr);
	close(sockets_ds->fd_tcp);
	free(sockets_ds);
}

void handle_signal_action(int sig_number) {
  if (sig_number == SIGINT) {
    printf("\nClosing server\n");
    cleanup_server();
    exit(EXIT_SUCCESS);
  }
}

// create directories that stores game related
// information GAMES and SCORES
void init_data(input_args args) {

	if(mkdir(GAME_DATA_DIR,S_IRWXU) == ERROR  && errno != EEXIST) {
		fprintf(stderr,ERROR_MKDIR,strerror(errno));
		
	}

	if(mkdir(GAMES_DIR,S_IRWXU) == ERROR  && errno != EEXIST) {
		fprintf(stderr,ERROR_MKDIR,strerror(errno));
		
	}
	
	if(mkdir(SCORES_DIR,S_IRWXU) == ERROR  && errno != EEXIST) {
		fprintf(stderr,ERROR_MKDIR,strerror(errno));
		
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

// Main program entry point
int main(int argc, char **argv) {

	pid_t pid;
	input_args args = parse_args(argc,argv);
	
	
	struct sigaction sa;
	sa.sa_handler = handle_signal_action;
  	if (sigaction(SIGINT, &sa, 0) != 0) {
    	perror("sigaction()");
    	exit(EXIT_FAILURE);
  	}
  	
  	sockets_ds = (socket_ds*) malloc(sizeof(socket_ds));
  	
	// initialize data storage
	init_data(args);
	
	file = fopen(args.word_file, "r");
	init_player_info(args, file);
	
	// create a child process
	pid = fork();
	// 0 is returned in the child
	if(pid == 0) {
		// child process responsible for handling udp requests
		
		udp_setup(sockets_ds, args);
		udp_request_handler(sockets_ds);

	} else if(pid > 0) {
		// calling process (parent)
		// responsible for handling tcp requests
		tcp_setup(sockets_ds, args);
		tcp_request_handler(sockets_ds);

	} else {
		// on failure - 1 is returned
		perror(ERROR_FORK);

		free(sockets_ds);

		exit(EXIT_FAILURE);
	}

}
