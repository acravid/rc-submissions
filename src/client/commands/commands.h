/*
 * File: commands.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for commands.c
*/

#ifndef COMMANDS_H
#define COMMANDS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/*---------------MACROS---------------*/
/*Maximum values: */
#define MAX_FILE_INFO_SIZE MAX_FILENAME + MAX_FILE_SIZE_DIGITS + 5
#define START_REQUEST_SIZE 11
#define START_RESPONSE_SIZE 12 + 1
#define PLAY_REQUEST_SIZE 15
#define PLAY_RESPONSE_SIZE 73
#define GUESS_REQUEST_SIZE 46
#define GUESS_RESPONSE_SIZE 10
#define SCOREBOARD_REQUEST_SIZE 4
#define HINT_REQUEST_SIZE 11
#define STATE_REQUEST_SIZE 11
#define QUIT_REQUEST_SIZE 11
#define QUIT_RESPONSE_SIZE 8

/*UDP error messages: */
#define ERROR_FD_UDP  "\n"\
					 "An error has occurred\n"\
					 "Failed to create an endpoint for communication\n"\
					 "UDP: The request (socket) was not satisfied\n"



#define ERROR_ADDR_UDP "\n"\
						   "An error has occurred\n"\
						   "UDP: The request (getaddrinfo) was not satisfied\n"

#define ERROR_SEND_UDP "\n"\
						"UDP: An error has occurred while trying to send data over SOCK_GRAM\n"\


#define ERROR_RECV_UDP "\n"\
					   "UDP: An error has occurred while trying to receive data over SOCK_GRAM\n"

/*TCP error messages: */
#define ERROR_FD_TCP  "\n"\
					 "An error has occurred\n"\
					 "Failed to create an endpoint for communication\n"\
					 "TCP: The request (socket) was not satisfied\n"

#define ERROR_TCP_CONNECT "\n"\
					 "An error has occurred\n"\
					 "TCP: The binding or connection was not successful\n"


#define ERROR_ADDR_TCP "\n"\
					  "An error has occurred\n"\
					  "TCP: The request (getaddrinfo) was not successful\n"
					  
#define ERROR_SEND_TCP "\n"\
						"TCP: An error has occurred while trying to send data over SOCK_STREAM\n"\

#define ERROR_RECV_TCP "\n"\
						"TCP: An error has occurred while trying to receive data over SOCK_STREAM\n"\

/*Client error messages: */
#define NO_GAME_ERROR "No game running.\n"
#define OPEN_FILE_ERROR "Error. Couldn't open file.\n"
#define SAVE_FILE_ERROR "Error. Couldn't save file.\n"

/*Server error messages: */
#define START_REQUEST_ERROR "Error. Player ID is invalid or has a game already running on the server.\n"
#define PLAY_REQUEST_ERROR  "Error. Server wasn't able to fulfill the play request.\n"
#define INVALID_TRIAL_ERROR "Error. Invalid trial number.\n"
#define GUESS_REQUEST_ERROR  "Error. Player ID is invalid or there is no game running on the server.\n"
#define QUIT_REQUEST_ERROR "Invalid player ID.\n"
#define EMPTY_SCOREBOARD_ERROR "Scoreboard is empty.\n"
#define NO_HINT_ERROR "The server has no hints for you.\n"
#define NO_SERVER_GAME_ERROR "Error. Invalid player ID or no games (active or finished) for that player in the server.\n"


/*Others: */
#define AUTO_PROTOCOL 0
enum status_code {OK, WIN, DUP, NOK, OVR, INV, ERR};

/*---------------Structs---------------*/

/* @brief:
    Structure that stores information related to SOCKET communication 
   	both DATAGRAM AND STREAM SOCKETS
*/
typedef struct {
    int fd_udp;
    int fd_tcp;
    struct addrinfo addrinfo_udp, *addrinfo_udp_ptr;
    struct addrinfo addrinfo_tcp, *addrinfo_tcp_ptr;
} socket_ds;

/*---------------UDP Function prototypes---------------*/
/* Setup: */
void udp_setup(socket_ds*,optional_args);

/*Requests: */
int send_start_request(socket_ds*, game_status*);
int send_play_request(socket_ds*, game_status*);
int send_guess_request(socket_ds*, game_status*);
int send_quit_request(socket_ds*, game_status*);

/*---------------TCP Function prototypes---------------*/
/* Setup: */
void tcp_setup(socket_ds*,optional_args);

/*Requests: */
int send_scoreboard_request(socket_ds*, optional_args, game_status*);
int send_hint_request(socket_ds*, optional_args, game_status*);
int send_state_request(socket_ds*, optional_args, game_status*);

/* Clean up socket resources: */
void cleanup_connection(int,struct addrinfo*);


#endif /* COMMANDS_H */