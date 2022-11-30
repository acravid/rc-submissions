/*
 * File: commands_udp.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for commands_udp.c
*/

#ifndef COMMANDS_UDP_H
#define COMMANDS_UDP_H


/*---------------Function prototypes---------------*/

/*Sending messages: */
int send_start_message();
int send_play_message();
int send_guess_message();
int send_quit_message();

/*Error handling: */
void handle_start_error();
void handle_play_error();
void handle_guess_error();
void handle_quit_error();
void handle_exit_error();

#endif /* COMMANDS_UDP_H */