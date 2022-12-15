/*
 * File: player.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for player.c
 * 
 * 
*/

#ifndef PLAYER_H
#define PLAYER_H

/*---------------MACROS---------------*/

/*Maximum values: */
#define MAX_STRING 65535
#define BUFFER_SIZE 512
#define MAX_FILENAME 28
#define MAX_FILE_SIZE_DIGITS 10

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
#define DEFAULT_GSIP "tejo.ist.utl.pt"
#define DEFAULT_GSPORT "58011"

/*Others: */
#define PROGRAM_IS_RUNNING 1
#define EQUAL 0
#define SUCCESS 0
#define ERROR -1
#define YES 0
#define NO 1
#define MAYBE -1
#define SCOREBOARD_PATHNAME "./"
#define SCOREBOARD_PATHNAME_SIZE 2
#define HINT_PATHNAME "./"
#define HINT_PATHNAME_SIZE 2
#define STATE_PATHNAME "./"
#define STATE_PATHNAME_SIZE 2


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
typedef struct {

    char *ip;
    char *port;

} optional_args;

typedef struct {
    
	char player_id[7];
    int letters;
    int errors;
    char* word;
    char* guess;
    char last_letter;
    int last_play;
    int running;
    int trial;
    char scoreboard_filename[MAX_FILENAME + SCOREBOARD_PATHNAME_SIZE];
    char hint_filename[MAX_FILENAME + HINT_PATHNAME_SIZE];
    char state_filename[MAX_FILENAME + STATE_PATHNAME_SIZE];
    char state_status[4];

} game_status;

void get_word(char*);
void upcase_word(char*);
#endif /* PLAYER_H */
