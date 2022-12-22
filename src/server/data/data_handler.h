/*
 * File: data_handler.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for data_handler.c
 */


#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include "../requests/request.h"

/*Function Prototypes*/

int select_random_word_hint(char*,char*,int);

void find_last_game(char*,char*);
void write_game_play(char*,char*,char*);
void create_game_play_txt(char*,char*); 
void write_game_play_to_file(char *,char*,char*);
void create_player_score_file(char*,float,float,char*,char*);
void create_player_game_directory(char*);
void rename_and_move_player_file(char*,char*,float,float);
void get_hint_filename(char*,char*);
void get_state_filename(char*,char*,char*);





// Macros

#define combine_path(path_a,path_b) path_a ## path_b


#define GAME_DATA "GAME_DATA"
#define GAMES_DATA_DIR "GAME_DATA/GAMES"
#define GAMES_SCORE_DIR "GAME_DATA/SCORES"
#define PLID_DIR "/%s"
#define GAME_FILE "/%s"
#define GAMES_DATA_ONGOING GAMES_DATA_DIR GAME_WRITE_PLAYER_FILE
#define GAMES_DATA_PLAYER_DIR GAMES_DATA_DIR PLID_DIR
#define GAME_DATA_PLAYER_DIR_AND_FILE GAMES_DATA_PLAYER_DIR GAME_FILE



#define PLAY_TRIAL_CODE "T "
#define WRITE_PLAY "%s%c\n"
#define PLAY_GUESS_CODE "G "
#define WRITE_GUESS "%s%s\n"
#define PLAY "PLG"
#define GUESS "PWG"
#define START "SNG"

#define TERMINATION_STATUS_WIN "W"
#define TERMINATION_STATUS_FAIL "F"
#define TERMINATION_STATUS_QUIT "Q"

#define DIR_FORWARD "/"

#define STATE_ACTIVE "ACT"
#define STATE_FIN "FIN"

#define GAME_PLAYER_PLAY "../SCORES/%s/%s"
#define GAME_WRITE_PLAYER "../GAMES/%s"
#define GAME_WRITE_PLAYER_FILE "/GAME_%s.txt"
#define SCORES_WRITE "../SCORES/%s"
#define FILE_EXTENSION ".txt"


#define READ_MODE "r"
#define WRITING_MODE "w+"
#define APPEND_MODE "a+"
#define APPEND_STANDARD "a"

#define TIME_FORMATTING "%Y%m%d_%H%M%S"
#define SCORE_UNITS_FORMATTING "00%d_%s_%s.txt"
#define SCORE_DOZENS_FORMATTNG "0%d_%s_%s.txt"
#define SCORE_FORMATTING "%d_%s_%s.txt"
#define SCORE_MESSAGE_FORMATTING_UNITS "00%d %s %s %d %d"
#define SCORE_MESSAGE_FORMATTING_DOZENS "0%d %s %s %d %d"
#define SCORE_MESSAGE_FORMATTING "%d %s %s %d %d"
#define WRITE_STRING "%s"
#define READ_STRING "%s %s"

#define TERMINATION_AND_EXTENSION_FORMATTING "_%s.txt"


#define NOT_DOZEN 10
#define NOT_DOZEN_ZEROS 3
#define DOZEN_ZEROS 2
#define NOT_HUNDRED 100
#define NOT_DOZEN_OFFSET "00"
#define DOZEN_OFFSET "0"
#define PERCENTAGE 100


#define SCORE_PARTIAL_PATH_LENGTH 66
#define SCORE_FULL_PATH_LENGTH 84
#define SCORE_MESSAGE_FULL_LENGTH 48
#define PATH_PLAYER_GAME_DIR_LENGTH 22
#define PATH_ONGOING_GAME_LENGTH 40
#define MAX_LINE_LENGTH 59
#define MAX_HINT_FILE_NAME_LENGHT 21
#define MAX_FILE_NAME_PATH_ON_GOING_GAME 24
#define MOVED_NAME_LENGTH 21
#define TIME_FORMATTING_LENGHT 50
#define DIR_NAME_LENGTH 20
#define MOVED_PLAY_FILE_LENGTH 44
#define TERMINATION_AND_EXTENSION_LENGTH 6
#define FILE_NAME_MAX_LENGTH 50
#define NUMBER_OF_LINES_GUESS_FILE 26
#define TEXT_FILE_LENGTH 64

// GAME_DATA/GAMES/GAME_6.txt
// GAME_DATA/

#define MAX_WORD_SIZE_HINT 31 

#define SUCESS 0
#define ERROR -1

#define ERROR_OPEN_FILE "\n"\
                        "An error has occurred, failed to open \n"
#define ERROR_MALLOC_FILE "\n"\
                        "An error has occurred, failed to allocate memory\n"
#define ERROR_MKDIR "\n"\
                    "mkdir(): an error has occurred, the directory was not created\n"
#define ERROR_RENAME "\n"\
                     "rename(): an error has occurred, failed to rename file\n"

#define ERROR_FPRINT "\n"\
                     "fprintf(): failed to send formatted output to file\n"
#define ERROR_CLOSE_FILE "\n"\
                         "An erros has occurred, failed to close file\n"

#define ERROR_UNKNOWN "\n"\
                       "An unknown error has occurred, please try again later\n"

#endif /* DATA_HANDLER_H */
