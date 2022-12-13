/*
 * File: common.h
 * Authors: Allan Fernandes 97281, João Vítor 99246
 * Description: header for common.c (socket communication|setup functions)
 * 
 * 
*/


#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>


struct addrinfo *getaddrinfo_extended(const char *ip, const char *port, int family, int socktype,int  protocol);
void create_mapping(protocol_status_code *protocol_status_code_array_ptr);
char *status_code_str(int status_code);
int status_code_int(char *status_code);


// Function Prototypes 



// Macros 
#define AUTO_PROTOCOL 0
#define SUCCEEDED 0
#define NUM_STATUS_CODE 7


// Macros for creating and manipulating buffers (generic buffers)

#define buffer_typedef(type,typename) \
    typedef struct {                  \
        size_t size;                  \
        type* info;                   \
    }typename;                        \

#define buffer_init(buf,len,type) \
    type info[len]; \
    buf.size = len; \
    buf.info = &info; 


// Note:
// this structure will be work just like a map 
//  
//     .----------.        .---------.
//     | int_code | < == > | argtype |
//     '----------'        '---------'
//
//     .-------------.        .---------.
//     | status_code | < == > | argtype |
//     '-------------'        '---------'
//
//
//
typedef struct {
    int int_code;
    const char *status_code;
    enum status_code argtype;

}protocol_status_code;

enum status_code {
    OK,
    WIN,
    DUP,
    NOK,
    OVR,
    INV,
    ERR
};




#endif  /* COMMON_H */