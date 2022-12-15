/*
 * File: game_server.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for game_server.c
 * 
 * 
*/

#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h> 
#include <stdbool.h> 

// Structure that stores information related to SOCKET communication 
// both DATAGRAM AND STREAM SOCKETS
typedef struct {

    int fd_udp;
    int fd_tcp;
    struct addrinfo addrinfo_udp, *addrinfo_udp_ptr;
    struct addrinfo addrinfo_tcp, *addrinfo_tcp_ptr;
	
} socket_ds; // socket DATAGRAM STREAM


typedef struct {

    char *port;
    bool verbose_flag;
    char *word_file;

} input_args;


#define DEFAULT_GSPORT "58091"
#define MAX_STRING 65535
#define EQUAL 0


/*---------------MACROS---------------*/

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
