/*
 * File: game_server.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for game_server.c
 * 
 * 
*/

#ifndef GAME_SERVER_H
#define GAME_SERVER_H

/*---------------MACROS---------------*/

// TODO:
#define USAGE_INFO  "\n"
					
/*---------------Struct---------------*/

// Structure that stores args used when calling the server
typedef struct {

    char *word_file;
    char *port;

} input_args;


#endif /* GAME_SERVER_H */
