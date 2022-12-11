/*
 * File: player.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for player.c
 * 
 * 
*/

#ifndef PLAYER_H
#define PLAYER_H


/*---------------Function prototypes---------------*/
/*Error handling: */
void handle_start_error();
void handle_play_error();
void handle_guess_error();
void handle_scoreboard_error();
void handle_hint_error();
void handle_state_error();
void handle_quit_error();
void handle_exit_error();

/*---------------MACROS---------------*/

/*Maximum values: */
#define MAX_STRING 65535

/*Commands: */
#define START_COMMAND "start"
#define SHORT_START_COMMAND "sg"
#define PLAY_COMMAND "play"
#define SHORT_PLAY_COMMAND "pl"
#define GUESS_COMMAND "guess"
#define SHORT_GUESS_COMMAND "gw"
#define SCOREBOARD_COMMAND "scoreboard"
#define SHORT_SCOREBOARD_COMMAND "sb"
#define HINT_COMMAND "hint"
#define SHORT_HINT_COMMAND "h"
#define STATE_COMMAND "state"
#define SHORT_STATE_COMMAND "st"
#define QUIT_COMMAND "quit"
#define EXIT_COMMAND "exit"

/*Default settings: */
#define DEFAULT_GSIP "tejo.tecnico.ulisboa.pt"
#define DEFAULT_GSPORT "58091"

/*Others: */
#define PROGRAM_IS_RUNNING 1
#define EQUAL 0
#define SUCESS 0
#define ERROR -1
#define ERROR_FD_TCP  "\n"\
					 "An error has occurred\n"\
					 "Failed to create an endpoint for communication\n"\
					 "TCP: The request (socket) was not satisfied\n"

#define ERROR_TCP_CONNECT "\n"\
					 "An error has occurred\n"\
					 "TCP: The binding or connection was not successful\n"

#define ERROR_FD_UDP  "\n"\
					 "An error has occurred\n"\
					 "Failed to create an endpoint for communication\n"\
					 "UDP: The request (socket) was not satisfied\n"

#define ERROR_ADDR_TCP "\n"\
					  "An error has occurred\n"\
					  "TCP: The request (getaddrinfo) was not successful\n"

#define ERROR_ADDRINFO_UDP "\n"\
						   "An error has occurred\n"\
						   "UDP: The request (getaddrinfo) was not satisfied\n"

		
#define USAGE_INFO  "\n"\
					"Player Application (Player)\n"\
		   			"Invalid arguments to start the player\n"\
		  			"Usage: ./player [-n GSIP] [ -p GSport]\n"\
		  			"\n"\
					"GSIP is the IP adress of the machine where the game server (GS) runs.\n"\
					"If this argument is omitted, the GS should be running on the same machine.\n"\
					"\n"\
					"GSPORT is the well-known port (TCP and UDP) where the GS accepts requests.\n"\
					"If omitted, it assumes the value 5800+GN, where GN is the group number\n"

/*---------------Struct---------------*/

/* @brief:
    struct that stores optional command line arguments
    if no arguments is given set to default.
*/
struct optional_args{

    char *ip;
    char *port;
};

#endif /* PLAYER_H */
