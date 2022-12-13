#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>


#include "common.h"

// TODO:
// complete function's brief

//
// Function:
//
//
// Inputs: 
//
//
// Description:
//
//
// 
struct addrinfo *getaddrinfo_extended(const char *ip, const char *port, int family, int socktype,int  protocol) {

    struct addrinfo addrinfo_t, *res;
    int ret;

    memset(&addrinfo_t,0,sizeof(addrinfo_t));
    // set hints
    addrinfo_t.ai_family = family;
    addrinfo_t.ai_socktype = socktype;
    addrinfo_t.ai_protocol = protocol;

    ret = getaddrinfo(ip,port,&addrinfo_t,&res);
    if(ret != SUCCEEDED) {
        return NULL;
    }

    return res;

}


// TODO:
// complete function's brief

//
// Function:
//
//
// Inputs: 
//
//
// Description:
//
//
// 
void create_mapping(protocol_status_code *protocol_status_code_array_ptr) {

    protocol_status_code_array_ptr[0].argtype = OK;
    protocol_status_code_array_ptr[0].int_code = 0;
    protocol_status_code_array_ptr[0].status_code = "OK";

    protocol_status_code_array_ptr[1].argtype = WIN;
    protocol_status_code_array_ptr[1].int_code = 1;
    protocol_status_code_array_ptr[1].status_code = "WIN";

    protocol_status_code_array_ptr[2].argtype = DUP;
    protocol_status_code_array_ptr[2].int_code = 2;
    protocol_status_code_array_ptr[2].status_code = "DUP";


    protocol_status_code_array_ptr[3].argtype = NOK;
    protocol_status_code_array_ptr[3].int_code = 3;
    protocol_status_code_array_ptr[3].status_code = "NOK";

    protocol_status_code_array_ptr[4].argtype = OVR;
    protocol_status_code_array_ptr[4].int_code = 4;
    protocol_status_code_array_ptr[4].status_code = "OVR";

    protocol_status_code_array_ptr[5].argtype = INV;
    protocol_status_code_array_ptr[5].int_code = 5;
    protocol_status_code_array_ptr[5].status_code = "INV";

    protocol_status_code_array_ptr[6].argtype = ERR;
    protocol_status_code_array_ptr[6].int_code = 6;
    protocol_status_code_array_ptr[6].status_code = "ERR";


}


// TODO:
// complete function's brief

//
// Function:
//
//
// Inputs: 
//
//
// Description:
//
//
// 
char *status_code_str(int status_code) {

    protocol_status_code all_status_code[NUM_STATUS_CODE];

    // Note:
    // protocol_status_code *protocol_status_code_array_ptr is a pointer to protocol_status_code. 
    // It could be a pointer to single instance of protocol_status_code, 
    // but it could also be a pointer to the first element of an array of instances of protocol_status_code:
    protocol_status_code *protocol_status_code_array_ptr;

    protocol_status_code_array_ptr = &all_status_code[0];


    create_mapping(protocol_status_code_array_ptr);

    return all_status_code[status_code].status_code;

}

int status_code_int(char *status_code) {

    protocol_status_code all_status_code[NUM_STATUS_CODE];
    protocol_status_code *protocol_status_code_array_ptr;
    protocol_status_code_array_ptr = &all_status_code[0];
    int i = NUM_STATUS_CODE - 1;


    create_mapping(protocol_status_code_array_ptr);


    while(i >= 0) {
        if(strcmp(all_status_code[i].status_code,status_code) == 0) {
            break;
        }
        i--;
    }
    return all_status_code[i].int_code;

}

// TODO:
// complete function's brief

//
// Function:
//
//
// Inputs: 
//
//
// Description:
//
//
// Note: return value's ssize_t since we're taking into account possibly returning 
// a negative value for indicating error -1
// ssize_t sendto(int __fd, const void *__buf, size_t __n, int __flags, const struct sockaddr *__addr, socklen_t __addr_len)
ssize_t send_udp_request(int fd_upd, char *buffer,size_t size,struct addrinfo *addrinfo_udp) {
    return sendto(fd_upd,buffer,size,0,addrinfo_udp->ai_addr,addrinfo_udp->ai_addrlen);

}


// TODO:
// complete function's brief

//
// Function:
//
//
// Inputs: 
//
//
// Description:
//
//
// Note: return value's ssize_t since we're taking into account possibly returning 
// a negative value for indicating error -1
// ssize_t recvfrom(int __fd, void *__restrict__ __buf, size_t __n, int __flags, struct sockaddr *__restrict__ __addr, socklen_t *__restrict__ __addr_len)
ssize_t recv_udp_response(int fd_udp, char *buffer, size_t size,struct addrinfo *addrinfo_udp) {
    return recvfrom(fd_udp,buffer,size - 1 ,0,addrinfo_udp->ai_addr,addrinfo_udp->ai_addrlen);

}

