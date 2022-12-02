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



#define AUTO_PROTOCOL 0
#define SUCCEEDED 0





#endif  /* COMMON_H */