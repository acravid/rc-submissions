#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "data_handler.h"


// create ongoing game file
void create_game_play_txt(char *plid,char *file_path) {

    sprintf(file_path,GAMES_DATA_ONGOING,plid);
    
    FILE *file = fopen(file_path,APPEND_STANDARD);
    if(file == NULL) {
        fprintf(stderr,ERROR_OPEN_FILE);   
    }

    fclose(file);
   
}


// create a directory game directory for a given player id
// stores finished game play files
void create_player_game_directory(char *plid) {

    char *dir_path = NULL;

    dir_path = (char*)malloc(sizeof(char) *PATH_PLAYER_GAME_DIR_LENGTH);
    sprintf(dir_path,GAMES_DATA_PLAYER_DIR,plid);
  
    if(dir_path == NULL) {
        fprintf(stderr,ERROR_MALLOC_FILE);
    }   

    if(mkdir(dir_path,S_IRWXU) == ERROR  && errno != EEXIST) {
		fprintf(stderr,ERROR_MKDIR,strerror(errno));
		
	}
    free(dir_path);

}


// obtain the time a file was last accessed 
// useful for file renaming and sorting purposes
void get_last_accessed_data_and_time(char *file_path, char *buffer) {

    struct stat time_attrib;
    stat(file_path, &time_attrib);
    struct tm* time = gmtime(&time_attrib.st_atime);     
    strftime(buffer,TIME_FORMATTING_LENGHT,TIME_FORMATTING,time);
    
}


// handle the creation of a player's game play score file 
// and writing of useful information to this same file
void create_player_score_file(char *plid,float n_succ,float n_trials,char *date_and_time,char *word) {

    char *score_filename = NULL;
    char *score_full_path = NULL;
    char *write_info = NULL;
    int temp  = 0;

    // calculate game score
    // NOTE: subtract 1 since our 'trial' is one-based (as defined in start())
    float score = (n_succ / ((int)n_trials - 1)) * PERCENTAGE;

    score_filename = (char*)malloc(sizeof(char) * SCORE_PARTIAL_PATH_LENGTH);
    score_full_path = (char*)malloc(sizeof(char) * SCORE_FULL_PATH_LENGTH);
    write_info = (char*)malloc(sizeof(char) * SCORE_MESSAGE_FULL_LENGTH);
    memset(write_info,'\0',sizeof(write_info));

    // downcast values back to integer
    // necessary since we'll later saved them to the score file as int
    temp = score;
    int n_succ_fixed = n_succ;
    int n_trials_without_start = n_trials - 1;

    // write both the filename (already formatted) and its formatted content to two separate buffers
    if(temp < NOT_DOZEN) {;
        sprintf(score_filename,SCORE_UNITS_FORMATTING,temp,plid,date_and_time);
        sprintf(write_info,SCORE_MESSAGE_FORMATTING_UNITS,temp,plid,word,n_succ_fixed,n_trials_without_start);
    } 
    else if (temp > NOT_DOZEN && score < NOT_HUNDRED) {
        sprintf(score_filename,SCORE_DOZENS_FORMATTNG,temp,plid,date_and_time);
        sprintf(write_info,SCORE_MESSAGE_FORMATTING_DOZENS,temp,plid,word,n_succ_fixed,n_trials_without_start);
    }
    else {
        sprintf(score_filename,SCORE_FORMATTING,temp,plid,date_and_time);
        sprintf(write_info,SCORE_MESSAGE_FORMATTING,temp,plid,word,n_succ_fixed,n_trials_without_start);
    }

    
    // obtain score file full path
    // full as starting from the defined base directory
    strcpy(score_full_path,GAMES_SCORE_DIR);
    strcat(score_full_path,DIR_FORWARD);
    strcat(score_full_path,score_filename);

    // creates the score file for a succesful finished game
    // NOTE: This file creation is always different 
    FILE *score_file = fopen(score_full_path,WRITING_MODE);
    if(score_file == NULL) {
        fprintf(stderr,ERROR_OPEN_FILE);   
    }


    // send formatted to the previously create score file
    if(fprintf(score_file,WRITE_STRING,write_info)!= (int)strlen(write_info)) {
        fprintf(stderr,ERROR_FPRINT);
        exit(EXIT_FAILURE);
    }
    
    fclose(score_file);

    free(score_filename);
    free(score_full_path);
    free(write_info);
  
}


// obtain selected guess word givnen the player's id
// and write it back to a buffer
void get_word_guess(char *plid, char *word) {

    char *file_path = NULL;
    char *line = NULL;
    char *hint = NULL;

    file_path = (char*)malloc(sizeof(char) * PATH_ONGOING_GAME_LENGTH);
    line = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);
    hint = (char*)malloc(sizeof(char) * MAX_WORD_SIZE_HINT);

    if(file_path == NULL || hint  == NULL || line == NULL) {
        fprintf(stderr,ERROR_MALLOC_FILE);
    }   

    // obtain ongoing game file path
    sprintf(file_path,GAMES_DATA_ONGOING,plid); 

    FILE *file = fopen(file_path,READ_MODE);
    if(file == NULL) {
        memset(word,'\0',sizeof(word));     
    }

    fgets(line,MAX_LINE_LENGTH,file);
    sscanf(line,READ_STRING,word,hint);
    fclose(file);

    free(file_path);
    free(line);
    free(hint);

}


// rename ongoing game file to moved name (name adopted once the game is finished)
// create score file if current game play was won
void rename_and_move_player_file(char *plid,char *termination_status,float n_succ,float n_trials) {

    char *current_file_path = NULL;
    char *moved_name = NULL; 
    char *date_and_time = NULL; 
    char *to_move_dir = NULL; 
    char *new_file_path = NULL; 
    char *termination_and_extension = NULL; 
    char *word = NULL;

    current_file_path = (char*)malloc(sizeof(char) * PATH_ONGOING_GAME_LENGTH);
    moved_name = (char*)malloc(sizeof(char) * MOVED_NAME_LENGTH);
    date_and_time = (char*)malloc(sizeof(char) * TIME_FORMATTING_LENGHT); 
    to_move_dir = (char*)malloc(sizeof(char) *PATH_PLAYER_GAME_DIR_LENGTH);
    new_file_path = (char*)malloc(sizeof(char) * MOVED_PLAY_FILE_LENGTH);
    termination_and_extension = (char*)malloc(sizeof(char) * TERMINATION_AND_EXTENSION_LENGTH);
    word = (char*) malloc(sizeof(char) * MAX_WORD_SIZE);

    memset(current_file_path,'\0',sizeof(current_file_path));
    memset(new_file_path,'\0',sizeof(new_file_path));

    // obtain ongoing game file path 
    sprintf(current_file_path,GAMES_DATA_ONGOING,plid); 

    // build moved file path
    sprintf(to_move_dir,GAMES_DATA_PLAYER_DIR,plid);
    strcpy(new_file_path,to_move_dir);
    strcat(new_file_path,DIR_FORWARD);

    // obtain last accessed time of ongoing game file before rename
    get_last_accessed_data_and_time(current_file_path,date_and_time);

    // obtain previously selected guess word and write it to a buffer
    get_word_guess(plid,word);

    // continue the build of moved file path
    sprintf(termination_and_extension,TERMINATION_AND_EXTENSION_FORMATTING,termination_status);
    strcpy(moved_name,date_and_time);
    strcat(new_file_path,moved_name);
    strcat(new_file_path,termination_and_extension);

    // rename the file that is:
    // move it to another directory and change its name
    if(rename(current_file_path,new_file_path) == ERROR) {
        fprintf(stderr,ERROR_RENAME);

    }

    // NOTE: SCORE directory files manipulation 
    // a scoreboard file is created for each game play that ends with success (WIN)
    // the same player can have several score files associated to different game plays

    // best scenario 
    // the player guess word the word correctly at start 
    // score = n_succ / n_trials * 100 ; score = 1/1 * 100  = 100

    // worst case scenario 
    // the player fails all tries except the last one, in which he correctly guess the word 
    // score = n_succ / n_trials * 100 ; score = 1 / total tries * 100  

    // verify if the finished game was successful
    if(strcmp(termination_status,TERMINATION_STATUS_WIN) == SUCCESS && n_succ != 0 && n_trials != 0) {
        create_player_score_file(plid,n_succ,n_trials,date_and_time,word);

    }

    free(current_file_path);
    free(moved_name);
    free(date_and_time);
    free(to_move_dir);
    free(new_file_path);
    free(termination_and_extension);
    free(word);

}


// write game play to file given as input
// NOTE: 3 different possible writes
// start game write - word hint_file
// play game write  - trial_code(T) letter
// guess game write - guess_code(G) word
void write_game_play(char *file_path, char *buffer,char *mode) {
    
    FILE *file = fopen(file_path,mode);
    if(file_path == NULL) {
        fprintf(stderr,ERROR_OPEN_FILE);
        exit(EXIT_FAILURE);

    }

    if(fprintf(file,WRITE_STRING,buffer)!= (int)strlen(buffer)) {
        fprintf(stderr,ERROR_FPRINT);
        exit(EXIT_FAILURE);
    }

    if(fclose(file) != 0) {
        fprintf(stderr,ERROR_CLOSE_FILE );
        exit(EXIT_FAILURE);
    }

}


// write the hint file name to a buffer given the player's id
void get_hint_filename(char *buffer,char *plid) {

    char *file_path = NULL;
    char *line = NULL;
    char *word = NULL;

    file_path = (char*)malloc(sizeof(char) * PATH_ONGOING_GAME_LENGTH);
    word = (char*)malloc(sizeof(char) * MAX_WORD_SIZE_HINT);
    line = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);

    if(file_path == NULL || word == NULL || line == NULL) {
        fprintf(stderr,ERROR_MALLOC_FILE);
    }   

    // obtain ongoing game file path
    sprintf(file_path,GAMES_DATA_ONGOING,plid); 

       
    FILE *file = fopen(file_path,READ_MODE);
    if(file == NULL) {
        // set buffer to null
        // useful for conditionals
        memset(buffer,'\0',sizeof(buffer));
        
    }

    fgets(line,MAX_LINE_LENGTH,file);
    sscanf(line,READ_STRING,word,buffer);

    free(file_path);
    free(line);
    free(word);

}


// obtain the formatted state file name 
void get_state_filename(char *plid, char *buffer, char* code) {

    char *ongoing_game_file_path = NULL;
    char *finished_game_file_path = NULL;

    // set buffer to null, useful for conditionals
    memset(buffer,'\0',sizeof(buffer));

    ongoing_game_file_path = (char*)malloc(sizeof(char) * PATH_ONGOING_GAME_LENGTH);
   
    // find ongoing game file path
    sprintf(ongoing_game_file_path,GAMES_DATA_ONGOING,plid);

    // check if file exists
    if(access(ongoing_game_file_path,F_OK) == SUCESS) {
    	strcpy(code,STATE_ACTIVE);
        strcpy(buffer,ongoing_game_file_path);
    }  
    else {
        
        find_last_game(plid,buffer);
        strcpy(code,STATE_FIN);

    }

    free(ongoing_game_file_path);

}

// handle call to write a game play
void write_game_play_to_file(char * plid,char *info,char *type) {

    char *file_path = NULL;
    char *write_info = NULL;
    
    file_path = (char*)malloc(sizeof(char) * PATH_ONGOING_GAME_LENGTH);
    write_info = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);

    if(file_path == NULL || write_info == NULL) {
        fprintf(stderr,ERROR_MALLOC_FILE);
    }   

    sprintf(file_path,GAMES_DATA_ONGOING,plid); 

    if(strcmp(type,PLAY) == SUCESS) {
        sprintf(write_info,WRITE_PLAY,PLAY_TRIAL_CODE,tolower(info[0])); 
        write_game_play(file_path,write_info,APPEND_MODE);        

    }
    else if(strcmp(type,GUESS) == SUCESS) {
	    sprintf(write_info,WRITE_GUESS,PLAY_GUESS_CODE,info);
	    write_game_play(file_path,write_info,APPEND_MODE);

    } else if(strcmp(type,START) == SUCESS) {
	    write_game_play(file_path,info,WRITING_MODE);

    }

    free(file_path);
    free(write_info);

}


// find the last played game given a player id and stores the 
// resulting filename in a variable pointed by fname.
void find_last_game(char *plid,char *fname) {

    struct dirent **filelist;
    int n_entries, found;
    char *dir_path = NULL;

    dir_path = (char*)malloc(sizeof(char) * PATH_PLAYER_GAME_DIR_LENGTH);
    sprintf(dir_path,GAMES_DATA_PLAYER_DIR,plid);
   
    n_entries = scandir(dir_path,&filelist,0,alphasort);
    found = 0;

    if(n_entries <= 0) {
        fprintf(stderr,ERROR_UNKNOWN);
        exit(EXIT_FAILURE);
    } 
    else {
        while(n_entries--) {

            if(filelist[n_entries]->d_name[0] != '.') {
                sprintf(fname,GAME_DATA_PLAYER_DIR_AND_FILE,plid,filelist[n_entries]->d_name);
                found = 1;
            }
            free(filelist[n_entries]);
            if(found) {
                break;
            }
      
        }
    }

    free(dir_path);

}