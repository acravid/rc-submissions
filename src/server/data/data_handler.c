#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

#include "data_handler.h"


// creates a path of the following format ../GAMES/GAMES_plid.txt
// and stores it to a variable pointed by file_path
char *create_game_play_txt(char *plid,char *file_path) {

    sprintf(file_path,GAME_WRITE_PLAYER_FILE,plid);
    
    FILE *file = fopen(file_path,"a");
    if(file == NULL) {
        fprintf(stderr,ERROR_OPEN_FILE,file_path);   
    }
    fclose(file);
    return file_path;

}

 
// randomly select a line that contains a word to be guessed and the associated hint file
// line format: word_to_guess hint_file.jpg
int select_random_word_hint(char *fname,char *buffer,int buffer_size) {

    int selected_line = rand() % NUMBER_OF_LINES_GUESS_FILE;
    int current_line = 0;

    FILE* to_guess = fopen(fname,"r");

    if(to_guess == NULL) {
        fprintf(stderr,ERROR_OPEN_FILE,fname);
    }
    
    while(fgets(buffer,buffer_size,to_guess) != NULL ) {
        current_line++;
        if(current_line == selected_line) break;
    }

    int buffer_length = strlen(buffer);
    // turn to str
    if(buffer[buffer_length -1] == '\n') {
        buffer[buffer_length - 1] = '\0';
    }

    return false;

}


// write the game play to the player game file
// NOTE: a player game file has the following format:
// GAME_plid.txt 
// a play can be a letter or guess 
// T l
// G word
int write_game_play(char *file_path, char *buffer,char *mode) {
    
    // the stream is positioned at the beginning of the file
    FILE *file = fopen(file_path,mode);
    if(file_path == NULL) {
        fprintf(stderr, "fopen(): failed to open file for writing");
        exit(EXIT_FAILURE);

    }

    if(fprintf(file,"%s",buffer)!= strlen(buffer)) {
        fprintf(stderr,"fprintf(): failed to transmit all bytes");
        exit(EXIT_FAILURE);
    }

    if(fclose(file) != 0) {
        fprintf(stderr, "fclose(): failed to close file");
        exit(EXIT_FAILURE);
    }
    return false;
}



// find the last played game given a player id and stores the 
// resulting filename in a variable pointed by fname.
int find_last_game(char *plid,char *fname) {

    struct dirent **filelist;
    int n_entries, found;
    char dirname[DIR_NAME_LENGTH];


    sprintf(dirname,GAME_WRITE_PLAYER,plid);
    n_entries = scandir(dirname,&filelist,0,alphasort);
    found = 0;

    if(n_entries <= 0) {
        return 0; 
    } 
    else {
        while(n_entries--) {
            if(filelist[n_entries]->d_name[0] != '.') {
                sprintf(fname,GAME_PLAYER_PLAY,plid,filelist[n_entries]->d_name);
                found = 1;
            }
            free(filelist[n_entries]);
            if(found) {
                break;
            }
      
        }
    }
    return found;

}