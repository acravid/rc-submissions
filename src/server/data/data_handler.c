#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>


#include "data_handler.h"


// creates a path of the following format GAME_DATA/GAMES/GAMES_plid.txt
// and stores it to a variable pointed by file_path
void create_game_play_txt(char *plid,char *file_path) {

    sprintf(file_path,GAMES_DATA_ONGOING,plid);
    
    FILE *file = fopen(file_path,"a");
    if(file == NULL) {
        fprintf(stderr,"fopen(): failed to open");   
    }

    fclose(file);
   
}


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



// write the game play to the player game file
// NOTE: a player game file has the following format:
// GAME_plid.txt 
// a play can be a letter or guess 
// T l
// G word
void write_game_play(char *file_path, char *buffer,char *mode) {
    
    // the stream is positioned at the beginning of the file
    FILE *file = fopen(file_path,mode);
    if(file_path == NULL) {
        fprintf(stderr, "fopen(): failed to open file for writing");
        exit(EXIT_FAILURE);

    }

    if(fprintf(file,"%s",buffer)!= (int)strlen(buffer)) {
        fprintf(stderr,"fprintf(): failed to transmit all bytes");
        exit(EXIT_FAILURE);
    }

    if(fclose(file) != 0) {
        fprintf(stderr, "fclose(): failed to close file");
        exit(EXIT_FAILURE);
    }

}

void write_game_play_to_file(char * player_id,char *info,char *type) {

    char *file_path = NULL;
    char *write_info = NULL;
    
    file_path = (char*)malloc(sizeof(char) * PATH_ONGOING_GAME_LENGTH);
    write_info = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);

    if(file_path == NULL || write_info == NULL) {
        fprintf(stderr,ERROR_MALLOC_FILE);
    }   
     
    // find ongoing game paths
    sprintf(file_path,GAMES_DATA_ONGOING,player_id); 

    if(strcmp(type,PLAY) == SUCESS) {
        sprintf(write_info,WRITE_PLAY,PLAY_TRIAL_CODE,tolower(info[0])); // write play formatted to buffer
        write_game_play(file_path,write_info,APPEND_MODE);               // write to player's file

    }
    else if(strcmp(type,GUESS) == SUCESS) {
	    sprintf(write_info,WRITE_GUESS,PLAY_GUESS_CODE,info);
	    write_game_play(file_path,write_info,APPEND_MODE);

    }

    free(file_path);
    free(write_info);
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