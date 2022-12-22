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


// TODO: description
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


// TODO: description
void get_last_accessed_data_and_time(char *file_path, char *buffer) {

    struct stat time_attrib;
    stat(file_path, &time_attrib);
    struct tm* time = gmtime(&time_attrib.st_atime);     
    strftime(buffer,TIME_FORMATTING_LENGHT,TIME_FORMATTING,time);
    
}


// TODO: description
void rename_and_move_player_file(char* player_id,char termination_status, player_info game) {

    char *current_file_path = NULL;
    char *moved_name = NULL; 
    char *date_and_time = NULL; 
    char *to_move_dir = NULL; 
    char *new_file_path = NULL; 
    char *termination_and_extension = NULL; 

    to_move_dir = (char*)malloc(sizeof(char) *PATH_PLAYER_GAME_DIR_LENGTH);
    current_file_path = (char*)malloc(sizeof(char) * PATH_ONGOING_GAME_LENGTH);
    moved_name = (char*)malloc(sizeof(char) * MOVED_NAME_LENGTH);
    date_and_time = (char*)malloc(sizeof(char) * TIME_FORMATTING_LENGHT); 
    new_file_path = (char*)malloc(sizeof(char) * MOVED_PLAY_FILE_LENGTH);
    termination_and_extension = (char*)malloc(sizeof(char) * TERMINATION_AND_EXTENSION_LENGTH);

    memset(current_file_path,sizeof(current_file_path), '\0');
    memset(new_file_path,sizeof(new_file_path),'\0');

    // find ongoing game paths
    sprintf(current_file_path,GAMES_DATA_ONGOING,player_id); 
    
    // shape original name
    sprintf(to_move_dir,GAMES_DATA_PLAYER_DIR,player_id);

    // shape moved_name
    strcpy(new_file_path,to_move_dir);
    strcat(new_file_path,DIR_FORWARD);

    // get date and time info to append to moved name
    get_last_accessed_data_and_time(current_file_path,date_and_time);

    // shape moved name
    sprintf(termination_and_extension,TERMINATION_AND_EXTENSION_FORMATTING,termination_status);
    strcpy(moved_name,date_and_time);
    strcat(new_file_path,moved_name);
    strcat(new_file_path,termination_and_extension);


    if(rename(current_file_path,new_file_path) == ERROR) {
        fprintf(stderr,ERROR_RENAME);

    }
 
    
    free(to_move_dir);
    free(current_file_path);
    free(moved_name);
    free(date_and_time);
    free(termination_and_extension);
    
    
    // SCOREBOARD PART
    
    int new_score = 0;
    char new_line[MAX_LINE_LENGTH];
    int score = 0;
    char lines[10][MAX_LINE_LENGTH];
    
    // see if win
    if (termination_status == TERMINATION_STATUS_WIN)
    	new_score = 100;
    
    //make newline with info
    sprintf(new_line, "%d %s %s %d %d\n", new_score, player_id, game.word, game.successful_trials, game.trial - 1);
    printf("%s\n", new_line);
    //TODO make this file if doesnt exist
    //open file
  	char scoreboard_file_path[strlen(SCORES_DATA_DIR) + strlen(SCOREBOARD_FILE) + 1];
  	sprintf(scoreboard_file_path, "%s/%s", SCORES_DATA_DIR, SCOREBOARD_FILE);
    FILE* scoreboard_file = fopen(scoreboard_file_path, "r+");
    
    if (scoreboard_file != NULL)
    	printf("abre o file\n");
  	
  	//TODO fix this
    int stop = -1;
    int counter = 0;
    for (; counter < 10; counter++) {
    
    	//reads lines from scoreboard
    	fgets(lines[counter], MAX_LINE_LENGTH, scoreboard_file);
    	//read score in that line
    	sscanf(lines[counter], "%d", &score);
    	
    	//set stop to the line you want to change
    	//if at EOF dont read more lines
    	if (feof(scoreboard_file)) {
    		stop = counter;
    		break;
    	}
    	else if (score < new_score && stop == -1)
    		stop = counter;
    }
    
    //back to the beginning of file
	rewind(scoreboard_file);
			
	int n = 0;
	for (int i = 0; i < counter; i++) {
		// change the line to newline
		if (i == stop) {
			printf("a\n");
			fprintf(scoreboard_file, new_line);
		}
		//leave the rest the same/lower them
		else {
			printf("b\n");
			fprintf(scoreboard_file, lines[n]);
			n += 1;
		}
    }
    
    fclose(scoreboard_file);

}





// write the game play to the player game file
// NOTE: a player game file has the following format:
// GAME_plid.txt 
// a play can be a letter or guess 
// T l
// G word
// TODO: description | Fix me
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



void get_hint_filename(char *buffer,char *player_id) {

    printf("Depois de entrar na get_hint_filename\n");

    char *file_path = NULL;
    char *line = NULL;
    char *word = NULL;

    file_path = (char*)malloc(sizeof(char) * PATH_ONGOING_GAME_LENGTH);
    word = (char*)malloc(sizeof(char) * MAX_WORD_SIZE_HINT);
    line = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);

    if(file_path == NULL || word == NULL || line == NULL) {
        fprintf(stderr,ERROR_MALLOC_FILE);
    }   

    // shape possible ongoing game filepath

    // find ongoing game paths
    sprintf(file_path,GAMES_DATA_ONGOING,player_id); 

       
    FILE *file = fopen(file_path,"r");
    if(file == NULL) {
        // set buffer to null 
        memset(buffer,strlen(buffer),'\0');
        
    }

    fgets(line,MAX_LINE_LENGTH,file);
    sscanf(line, "%s %s",word,buffer);

    printf("o ficheiro e:");
    printf("%s",buffer);
    

    free(file_path);
    free(line);
    free(word);

}


void get_state_filename(char *player_id, char *buffer, char* code) {

    char *ongoing_game_file_path = NULL;
    char *finished_game_file_path = NULL;

    // set buffer to null, useful for conditionals
    memset(buffer,strlen(buffer),'\0');


    ongoing_game_file_path = (char*)malloc(sizeof(char) * PATH_ONGOING_GAME_LENGTH);
   
    // find ongoing game file path
    sprintf(ongoing_game_file_path,GAMES_DATA_ONGOING,player_id);
    // check if file exists
    if(access(ongoing_game_file_path,F_OK) == SUCESS) { //file exists
    	strcpy(code, "ACT");
        strcpy(buffer,ongoing_game_file_path);
        printf("filepath %s\n",buffer);
    }  
    else {

        // we can look for it at another directory
        find_last_game(player_id,buffer);
        strcpy(code, "FIN");
        printf("moved file:\n");
        printf("%s",buffer);
        if(buffer == NULL) {
            printf("to think\n");
        }

    }

}

// TODO: description
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
        // does nothing
        printf("does nothing");
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