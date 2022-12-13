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


// Function Prototypes 



// Macros 
#define AUTO_PROTOCOL 0
#define SUCCEEDED 0

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



#endif  /* COMMON_H */