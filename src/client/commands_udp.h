/*
 * File: commands_udp.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for commands_udp.c
*/

#ifndef COMMANDS_UDP_H
#define COMMANDS_UDP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
/*---------------Function prototypes---------------*/

/*Sending messages: */
int send_start_message();
int send_play_message();
int send_guess_message();
int send_quit_message();


struct socket_udp {
    
    int udp_fd;
    struct addrinfo *udp_id;
};




#endif /* COMMANDS_UDP_H */