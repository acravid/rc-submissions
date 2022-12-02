#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>


#include "common.h"


struct addrinfo *getaddrinfo_extended(const char *ip, const char *port, int family, int socktype,int  protocol) {

    int error_code;
    struct addrinfo addrinfo_t, *res;

    memset(&addrinfo_t,0,sizeof(addrinfo_t));
    // set hints
    addrinfo_t.ai_family = family;
    addrinfo_t.ai_socktype = socktype;
    addrinfo_t.ai_protocol = protocol;

    error_code = getaddrinfo(ip,port,&addrinfo_t,&res);
    if(error_code != SUCCEEDED) {
        return NULL;
    }

    return res;

}