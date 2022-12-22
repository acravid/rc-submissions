/*
 * File: player.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for player.c
*/

#ifndef PLAYER_H
#define PLAYER_H

/*---------------MACROS---------------*/

/*Default settings: */
#define DEFAULT_GSIP "tejo.ist.utl.pt"
#define DEFAULT_GSPORT "58011"

/*Maximum values: */
#define MAX_FILENAME 28
#define MAX_FILE_SIZE_DIGITS 10
#define SOCKET_TIMEOUT 30
#define MAX_TIMEOUTS 2

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

/*Pathnames: */     
#define SCOREBOARD_PATHNAME "./client/RECEIVED_FILES/"
#define SCOREBOARD_PATHNAME_SIZE 25
#define HINT_PATHNAME "./client/RECEIVED_FILES/"
#define HINT_PATHNAME_SIZE 25
#define STATE_PATHNAME "./client/RECEIVED_FILES/"
#define STATE_PATHNAME_SIZE 25
#define RECEIVED_FILES_DIR "client/RECEIVED_FILES"

/*Messages: */
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

#define QUIT_MESSAGE "Game quitted.\n"

#define ERROR_MKDIR_PLAYER "mkdir() an error has occurred, the directory was not created\n"

/*Others: */
#define PROGRAM_IS_RUNNING 1
#define EQUAL 0
#define SUCCESS 0
#define ERROR -1
#define YES 0
#define NO 1
#define MAYBE -1

/*---------------Structs---------------*/

/* @brief:
    Struct that stores optional command line arguments
*/
typedef struct {
    char *ip;
    char *port;
} optional_args;

/* @brief:
    Struct that stores relevant game data
*/
typedef struct {
	int running;
	
	char player_id[7];
    int n_letters;
    int n_errors;
    char* word;
    char* guess;
    char last_letter;
    int last_play;
    int trial;
    
    char scoreboard_filename[MAX_FILENAME + SCOREBOARD_PATHNAME_SIZE];
    char hint_filename[MAX_FILENAME + HINT_PATHNAME_SIZE];
    char state_filename[MAX_FILENAME + STATE_PATHNAME_SIZE];
    char state_status[4];
} game_status;

/*---------------Function prototype---------------*/

void get_word(char*, int);

#endif /* PLAYER_H */
