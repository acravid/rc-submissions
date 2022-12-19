#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>


int select_random_word(char *fd, char *buffer,int buffer_size) {


    int selected_line = rand() % 26;
    int current_line = 0;


    
    FILE* word_eng = fopen(fd,"r");

    if(word_eng == NULL) {
        fprintf(stderr,"An has occurred, failed to open %s\n",fd);
    }
    

    while(fgets(buffer,buffer_size,word_eng) != NULL ) {
        current_line++;
        if(current_line == selected_line) break;
    }

    int buffer_size = strlen(buffer);
    // turn to str
    if(buffer[buffer_size -1] == '\n') {
        buffer[buffer_size - 1] = '\0';
    }

    return 0;

}


