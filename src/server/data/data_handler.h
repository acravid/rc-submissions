/*
 * File: data_handler.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for data_handler.c
 * 
 * 
 */


#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H


/*Function Prototypes*/

int select_random_word_hint(char*,char*,int);
int find_last_game(char*,char*);

void write_game_play(char *file_path, char *buffer,char *mode);
void create_game_play_txt(char*,char*); 
void write_game_play_to_file(char *,char*,char*);
void create_player_game_directory(char*);


// Macros

#define combine_path(path_a,path_b) path_a ## path_b


#define GAME_DATA "GAME_DATA"
#define GAMES_DATA_DIR "GAME_DATA/GAMES"
#define SCORES_DATA_DIR "GAME_DATA/SCORES"
#define PLID_DIR "/%s"
#define GAMES_DATA_ONGOING GAMES_DATA_DIR GAME_WRITE_PLAYER_FILE
#define GAMES_DATA_PLAYER_DIR GAMES_DATA_DIR PLID_DIR


#define PLAY_TRIAL_CODE "T "
#define WRITE_PLAY "%s%c\n"
#define PLAY_GUESS_CODE "G "
#define WRITE_GUESS "%s%s\n"
#define PLAY "PLG"
#define GUESS "PWG"


#define GAME_PLAYER_PLAY "../SCORES/%s/%s"
#define GAME_WRITE_PLAYER "../GAMES/%s"
#define GAME_WRITE_PLAYER_FILE "/GAME_%s.txt"
#define SCORES_WRITE "../SCORES/%s"


#define WRITING_MODE "w+"
#define APPEND_MODE "a+"


#define PATH_PLAYER_GAME_DIR_LENGTH 22
#define PATH_ONGOING_GAME_LENGTH 40
#define MAX_LINE_LENGTH 45
#define MAX_HINT_FILE_NAME_LENGHT 21
#define MAX_FILE_NAME_PATH_ON_GOING_GAME 24
#define DIR_NAME_LENGTH 20
#define FILE_NAME_MAX_LENGTH 50
#define NUMBER_OF_LINES_GUESS_FILE 26
#define TEXT_FILE_LENGTH 64

#define SUCESS 0
#define ERROR -1

#define ERROR_OPEN_FILE "An error has occurred, failed to open %s\n"
#define ERROR_MALLOC_FILE "An error has occurred, failed to allocate memory\n"
#define ERROR_MKDIR "\n"\
                    "mkdir() an error has occurred, the directory was not created\n"

#endif /* DATA_HANDLER_H */