/*
 * File: player.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for player.c
*/

#ifndef PLAYER_H
#define PLAYER_H

/*---------------Struct---------------*/



/* @brief:
    struct that stores optional command line arguments
    if no arguments is given set to default.
*/
struct optional_args{

    char *ip;
    int port;
};

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
#define DEFAULT_GSPORT 58091

/*Others: */
#define PROGRAM_IS_RUNNING 1
#define EQUAL 0
#define ERROR -1


#endif /* PLAYER_H */