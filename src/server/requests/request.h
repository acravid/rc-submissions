/*
 * File: requests.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for requests.c
*/

#ifndef REQUESTS_H
#define REQUESTS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h> 
#include <errno.h>
#include "../GS.h"

/*---------------MACROS---------------*/
/*Maximum values: */
#define MAX_WORD_SIZE 31
#define CLIENT_UDP_MAX_REQUEST_SIZE 46 // guess request
#define SERVER_UDP_MAX_RESPONSE_SIZE 73 // play response
#define HINT_REQUEST_SIZE 11
#define MAX_GUESS_REPLY_SIZE 11
#define MAX_PLAY_REPLY_SIZE 76
#define CODE_SIZE 3
#define CLIENT_TCP_MAX_REQUEST_SIZE 11
#define SERVER_TCP_MAX_REPLY_SIZE MAX_FILENAME + MAX_FILE_SIZE_DIGITS + MAX_FILE_SIZE + 13
#define MAX_FILE_SIZE 1024 * 1024
#define MAX_HINT_REPLY_SIZE MAX_FILENAME + MAX_FILE_SIZE_DIGITS + MAX_FILE_SIZE + 2
#define MAX_SCOREBOARD_REPLY_SIZE MAX_FILENAME + MAX_FILE_SIZE_DIGITS + MAX_FILE_SIZE + 2
#define MAX_STATE_REPLY_SIZE MAX_FILENAME + MAX_FILE_SIZE_DIGITS + MAX_FILE_SIZE + 2
#define PLAYERID_MIN 90000
#define PLAYERID_MAX 110000
#define PLAYERID_SIZE 6
#define MAX_QUEUED_REQUESTS 10

/*Protocol codes: */
/*UDP: */
#define START_CODE "SNG"
#define PLAY_CODE "PLG"
#define GUESS_CODE "PWG"
#define QUIT_CODE "QUT"
#define DEBUG_CODE "REV"
#define START_REPLY_CODE "RSG"
#define PLAY_REPLY_CODE "RLG"
#define GUESS_REPLY_CODE "RWG"
#define QUIT_REPLY_CODE "RQT"

/*TCP: */
#define SCOREBOARD_CODE "GSB"
#define HINT_CODE "GHL"
#define STATE_CODE "STA"
#define SCOREBOARD_REPLY_CODE "RSB"
#define HINT_REPLY_CODE "RHL"
#define STATE_REPLY_CODE "RST"

/*Status: */
#define OK_REPLY_CODE "OK"
#define NOK_REPLY_CODE "NOK"
#define WIN_REPLY_CODE "WIN"
#define OVR_REPLY_CODE "OVR"
#define DUP_REPLY_CODE "DUP"
#define INV_REPLY_CODE "INV"
#define EMPTY_REPLY_CODE "EMPTY"
#define ERROR_REPLY_CODE "ERR"

/*Messages: */
/*UDP error messages: */
#define ERROR_FD_UDP "An error has occurred\n"\
					 "UDP: The request (socket) was not satisfied\n"

#define ERROR_ADDR_UDP "An error has occurred\n"\
						"UDP: The request (getaddrinfo) was not satisfied\n"

#define ERROR_BIND_UDP "An error has occurred\n"\
						"UDP: The request (bind) was not satisfied\n"

#define TIMEOUT_SEND_UDP "Error sending udp request. Trying again.\n"

#define TIMEOUT_RECV_UDP "Error receiving udp response. Trying again.\n"

#define ERROR_RECV_FROM "recvfrom(): an error has occurred, failed to receive message from socket\n"

#define ERROR_SEND_TO "sendto(): an error has occurred, failed to send message on socket\n"

/*TCP error messages: */
#define ERROR_FD_TCP  "An error has occurred\n"\
					 "TCP: socket() was not satisfied\n"

#define ERROR_ADDR_TCP "An error has occurred\n"\
					  "TCP:  getaddrinfo() was not successful\n"

#define ERROR_BIND_TCP "An error has occurred\n"\
					   "TCP: bind() was not successful\n"

#define ERROR_LISTEN "An error has occurred\n"\
					   "TCP: listen() was not successful\n"
		
#define TIMEOUT_SEND_TCP "Error sending tcp request. Trying again.\n"

#define TIMEOUT_RECV_TCP "Error receiving tcp response. Trying again.\n"

#define ERROR_READ "read(): an erros has occurred, failed to read\n"

#define ERROR_WRITE "write(): an erros has occurred, failed to write\n"

#define ERROR_ACCEPT "accept(): an error has occurred, failed to accept connection on socket\n"

/*Other error messages: */
#define ERROR_CLOSE "close(): an error has occurred, failed to close the file\n"

/*Others: */
#define AUTO_PROTOCOL 0
#define FORK_CHILD 0
#define ERROR -1
#define SUCCESS 0
#define ASCII_A 65
#define ASCII_z 122

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

/* @brief:
    Struct that stores relevant game data
*/
typedef struct {
	char word[MAX_WORD_SIZE];
	int n_letters;
	int n_errors;
	char played_letters[27];
	int trial;
	int successful_trials;
	char last_request[CLIENT_UDP_MAX_REQUEST_SIZE];
	char last_response[SERVER_UDP_MAX_RESPONSE_SIZE];
} player_info;

/*---------------Function prototypes---------------*/
void init_player_info(input_args, FILE*);

/*---------------UDP Function prototypes---------------*/
/* Setup: */
void udp_setup(socket_ds*, input_args);

/*Requests: */
void udp_request_handler(socket_ds*);
void start_request_handler(char*,size_t,char*);
void play_request_handler(char*,size_t,char*);
void guess_request_handler(char*,size_t,char*);
void quit_request_handler(char*,size_t,char*);
void exit_request_handler(char*,size_t,char*);
void debug_request_handler(char*,size_t,char*);

/*---------------TCP Function prototypes---------------*/
/* Setup: */
void tcp_setup(socket_ds*, input_args);

/*Requests: */
void tcp_request_handler(socket_ds*);
void scoreboard_request_handler(char*,size_t,char*);
void hint_request_handler(char*,size_t,char*);
void state_request_handler(char*,size_t,char*);

/* Clean up socket resources: */
void cleanup_connection(int, struct addrinfo*);

#endif /* REQUEST_H */