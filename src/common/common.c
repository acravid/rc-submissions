#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>


#include "common.h"


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

