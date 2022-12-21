/*
 * File: game_server.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for game_server.c
 * 
 * 
*/

#ifndef GS_H
#define GS_H

#include <stdbool.h> 

typedef struct {
    char *port;
    bool verbose_flag;
    char *word_file;
} input_args;


//--------------------------------------------------------------
//                              MACROS
//--------------------------------------------------------------

#define DEFAULT_GSPORT "58091"
#define MAX_STRING 65535
#define EQUAL 0
#define SUCCESS 0
#define MAX_FILENAME 28
#define MAX_FILE_SIZE_DIGITS 10
#define HINT_FILE_PATH "./hints/"
#define SCOREBOARD_FILE_PATH "./"
#define SCOREBOARD_FILE_NAME "scoreboard.txt"
#define STATE_FILE_PATH "./"
#define MAX_FILE_NAME 28
#define SOCKET_TIMEOUT 10




#define ERROR_MKDIR "\n"\
                    "mkdir() an error has occurred, the directory was not created\n"

#define ERROR_FORK "\n"\
                    "fork() an error has occurred, failed to create a child process\n"



// TODO:
#define USAGE_INFO  "\n"\
					"Game Server (GS)\n"\
		   			"Invalid arguments to start the server\n"\
		  			"Usage: ./GS word_file [-p GSport] [ -v ]\n"\
		  			"\n"\
                    "\n"\
                    "word_file is the name of a file containing a set of words that the GS\n"\
                    "can select from when a new game is started.\n"\
                    "Each line of the file contains a word a class to which the word belongs "\
                    "by a single spoace.\nThis file is located in the same directory as the GS"\
                    "executable"\
                    "\n"\
                    "\n"\
                    "GSport is the well-known port where the GS server accepts requests, both in UDP and TCP.\n"\
                    "This is an optional argument.If omitted, it assumes the value 58000+GN "\
                    "where GN is the number of the group.\n"\
                    "\n"\
                    "\n"\
                    "The GS makes available two server applications, both with well-known port GSport\n"\
                    "one in UDP, used for playing the game, and the other in TCP, used to transfer the\n"\
                    "scoreboard text file and the game logo image file to the Player application.\n"\
                    "If the –v option is set when invoking the program, it operates in verbose mode, meaning\n"\
                    "that the GS outputs to the screen a short description of the received requests (PLID"\
                    "type of request)\nand the IP and port originating those requests."\
                    "Each received request should start being processed once it is received" 
                  

					





#endif /* GAME_SERVER_H */
