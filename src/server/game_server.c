#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_server.h"

// TODO 

// 1.function that establishes udp connection
// 2. function that establishes tcp connection
// 3. function that handles commands from udp
// 4. function that handles commands from tcp
// 5... function that process each of the request
// 6 function that handles input parsing



//
// Function:
//
//
// Inputs: 
//
//
// Description:
//
//
//
// prints usage message to stderr
static void usage() {
	fprintf(stderr, USAGE_INFO);
}


//
// Function: 
//
//
// Inputs: 
//
//
// Description:
//
//
input_args parse_args(int argc, char **argv) {

    if(argc == 1 || argc > 5) {
	    usage();
	    exit(EXIT_FAILURE);
    }

  	input_args args = { .port = DEFAULT_GSPORT , .verbose_flag = false };

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

	input_args args = parse_args(argc,argv);

	exit(EXIT_SUCCESS);

}
