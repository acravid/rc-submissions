/*
 * File: commands_tcp.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for commands_tcp.c
*/

#ifndef COMMANDS_TCP_H
#define COMMANDS_TCP_H


/*---------------Function prototypes---------------*/

/*Sending messages: */
int send_scoreboard_message();
int send_hint_message();
int send_state_message();


struct socket_tcp {
    
    int tcp_fd;
    struct addrinfo *tcp_id;
};


#endif /* COMMANDS_TCP_H */
