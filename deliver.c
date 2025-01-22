#include <stdio.h>

int main(int argc, char *argv[]) {
    if( argc != 3){
        printf("Error - Both a Server Address and a Server Port Number are Required.\n");
        return 0;
    
    } else {
        printf("Server Address: %s\n", argv[1]);
        printf("Server Port Number: %s\n", argv[2]);
    }

    printf("Hello World! - deliver\n");

    //getting the current ip address
    //first you set up the specifications with hints and then prepare the recepticle with servinfo
    //then we do inet_ntop to convert
    // int status;
    // struct addrinfo hints;
    // struct addrinfo *servinfo; 

    // memset(&hints, 0, sizeof hints); 
    // hints.ai_family = AF_INET; 
    // hints.ai_socktype = SOCK_DGRAM; 
    // hints.ai_flags = AI_PASSIVE; 

    // if ((status = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
    //     return errorCheck("getaddrinfo error");
    // }

    // struct sockaddr_in *ipv4 = (struct sockaddr_in *)servinfo->ai_addr;
    // void *addr = &(ipv4->sin_addr);
    // char ipstr[INET6_ADDRSTRLEN];
    // inet_ntop(servinfo->ai_family, addr, ipstr, sizeof ipstr);

    return 0;
}