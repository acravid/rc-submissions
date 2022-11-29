/*
 * File: commands_udp.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for commands_udp.c
*/

#ifndef COMMANDS_UDP_H
#define COMMANDS_UDP_H

#include "player.h"

/*---------------Function prototypes---------------*/

/*Sending messages: */
int send_start_message(struct, char*);
int send_play_message(struct, char*);
int send_guess_message(struct, char*);
int send_quit_message(struct);

/*Error handling: */
void handle_start_error();
void handle_play_error();
void handle_guess_error();
void handle_quit_error();


#endif /* COMMANDS_UDP_H */