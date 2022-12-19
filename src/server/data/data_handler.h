/*
 * File: data_handler.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for data_handler.c
 * 
 * 
 */


#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H



// game data paths 
#define SCORES_DIR "../SCORES/"
#define GAMES_DIR "../GAMES/"

#define GAME_PLAYER_PLAY "../SCORES/%s/%s"
#define GAME_WRITE_PLAYER "../GAMES/%s"
#define GAME_WRITE_PLAYER_FILE "../GAMES/GAMES_%s.txt"
#define SCORES_WRITE "../SCORES/%s"



int select_random_word_hint(char*,char*,int);
int find_last_game(char*,char*);
int write_play_start(char *player_id, char *buffer);


// Macros

#define DIR_NAME_LENGTH 20
#define FILE_NAME_MAX_LENGTH 50
#define NUMBER_OF_LINES_GUESS_FILE 26
#define TEXT_FILE_LENGTH 64

#define ERROR_OPEN_FILE "An has occurred, failed to open %s\n"
#endif /* DATA_HANDLER_H */

