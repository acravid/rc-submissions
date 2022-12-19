#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

#include "data_handler.h"



// randomly select a line that contains a word to be guessed and the associated hint file
// line format: word_to_guess hint_file.jpg
int select_random_word_hint(char *fname,char *buffer,int buffer_size) {

    int selected_line = rand() % NUMBER_OF_LINES_GUESS_FILE;
    int current_line = 0;

    FILE* to_guess = fopen(fname,"r");

    if(to_guess == NULL) {
        fprintf(stderr,ERROR_OPEN_FILE,fname);
    }
    
    while(fgets(buffer,buffer_size,fname) != NULL ) {
        current_line++;
        if(current_line == selected_line) break;
    }

    int buffer_length = strlen(buffer);
    // turn to str
    if(buffer[buffer_length -1] == '\n') {
        buffer[buffer_length - 1] = '\0';
    }

    return 0;

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


