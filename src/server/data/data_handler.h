/*
 * File: data_handler.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for data_handler.c
 * 
 * 
 */


#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H


typedef struct {


} scorelist;


// game data paths 
#define SCORES_DIR "../SCORES/"
#define GAMES_DIR "../GAMES/"

#define GAME_PLAYER_PLAY "../SCORES/%s/%s"
#define GAME_WRITE_PLAYER "../GAMES/%s"
#define SCORES_WRITE "../SCORES/%s"

int select_random_word_hint(char*,char*,int);
int find_last_game(char*,char*);


// Macros

#define DIR_NAME_LENGTH 20
#define FILE_NAME_MAX_LENGTH 50
#define NUMBER_OF_LINES_GUESS_FILE 26



#define ERROR_OPEN_FILE "An has occurred, failed to open %s\n"
#endif /* DATA_HANDLER_H */

