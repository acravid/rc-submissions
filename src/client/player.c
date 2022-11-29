/*
 * File: client.c
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: player application for a remote word game
*/

#include <stdio.h>
#include <stdlib.h>

/*---------------Constants---------------*/

/*Maximum values: */
#define MAX_STRING 65535

/*Errors: */


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
#define DEFAULT_GSPORT 58011

/*Others: */
#define PROGRAM_IS_RUNNING 1
#define EQUAL 0
	
/*---------------Function prototypes---------------*/

/*Sending messages: */
int send_start_message(char*);
int send_play_message(char*);
int send_guess_message(char*);
int send_scoreboard_message();
int send_state_message();
int send_quit_message();

/*Error handling: */
handle_start_error();
handle_play_error();
handle_guess_error();
handle_scoreboard_error();
handle_state_error();
handle_quit_error();
handle_exit_error();

/*---------------Global variables---------------*/
char* GSIP = DEFAULT_GSIP;
int GSport = DEFAULT_GSPORT;

/*---------------Functions---------------*/

int main(int argc, char **argv) {
	
	if (argc == 2 || argc == 4 || argc > 5) {
		printf("Invalid arguments to start the player\n");
		exit(1);
    }
    if (argc >= 3) {
    	if (cmp_str(argv[1], "-n") == EQUAL)
    		GSIP = argv[2];
    	else if (cmp_str(argv[1], "-p") == EQUAL)
    		GSport = argv[2];
    	else {
    		iprintf("Invalid arguments to start the player\n");
			exit(1);
		}
    }
    if (argc == 5) {
    	if (cmp_str(argv[3], "-n") == EQUAL) 
    		GSIP = argv[4];
    	else if (cmp_str(argv[3], "-p") == EQUAL)
    		GSport = argv[4];
    	else {
    		iprintf("Invalid arguments to start the player\n");
			exit(1);
		}
    }


    /* creates a UDP and TCP connection with the server*/
    /* TO DO */
    

	char input_line[MAX_STRING]; /*receives the input*/
	char* command; /*receives the command from  stdin*/

	/*reads commands indefinitely until exit is given*/
	while (PROGRAM_IS_RUNNING) {

		/* reads from sdin*/
		fgets(input_line, MAX_STRING, stdin);
		/*separates the command from the rest of the input*/
		command = strtok(input_line, " ");

		if (cmp_str(command, START_COMMAND) == EQUAL || cmp_str(command, SHORT_START_COMMAND) == EQUAL) {
			if (send_start_message(input_line) == ERROR)
				handle_start_error(); 
		}
		else if (cmp_str(command, PLAY_COMMAND) == EQUAL || cmp_str(command, SHORT_PLAY_COMMAND) == EQUAL) {
			if (send_play_message(input_line) == ERROR)
				handle_play_error(); 
		}
		else if (cmp_str(command, GUESS_COMMAND) == EQUAL || cmp_str(command, SHORT_GUESS_COMMAND) == EQUAL) {
			if (send_guess_message(input_line) == ERROR)
				handle_guess_error(); 
		}
		else if (cmp_str(command, SCOREBOARD_COMMAND) == EQUAL || cmp_str(command, SHORT_SCOREBOARD_COMMAND) == EQUAL) {
			if (send_scoreboard_message() == ERROR)
				handle_scoreboard_error(); 
		}
		else if (cmp_str(command, HINT_COMMAND) == EQUAL || cmp_str(command, SHORT_HINT_COMMAND) == EQUAL) {
			if (send_hint_message() == ERROR)
				handle_hint_error(); 
		}
		if (cmp_str(command, STATE_COMMAND) == EQUAL || cmp_str(command, SHORT_STATE_COMMAND) == EQUAL) {
			if (send_state_message() == ERROR)
				handle_state_error(); 
		}
		if (cmp_str(command, QUIT_COMMAND) == EQUAL) {
			if (send_quit_message() == ERROR)
				handle_quit_error(); 
		}
		if (cmp_str(command, EXIT_COMMAND) == EQUAL) {
			if (send_quit_message == ERROR)
				handle_exit_error();
			break; 
		}
	}

	return 0;
}