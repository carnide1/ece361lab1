#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

//for checking errors
int errorCheck(char * message){
    int err = errno;
    perror(message);
    return err;
}

int main(int argc, char *argv[]) {
    //checking if the correct arguments were given
    if (argc != 2) { 
        printf("Error - Port Number Required.\n"); 
        return 0;
    } else { 
        printf("Port Number: %s\n", argv[1]); 
    }    

    //check if the port is valid
    int portNum = atoi(argv[1]);

    //open a socket
    int sock = socket(AF_INET,  SOCK_DGRAM, 0);

    if(sock == -1){
        return errorCheck("Socket Open Error");
    } else { //REMOVE
        printf("Socket opened! -> %d\n", sock);
    }

    //setting the sockaddr type
    struct sockaddr_in sockAddy;
    memset(&sockAddy, 0, sizeof(sockAddy));
    sockAddy.sin_family = AF_INET;
    sockAddy.sin_port = htons(atoi(argv[1]));
    sockAddy.sin_addr.s_addr = htonl(INADDR_ANY);

    struct sockaddr * psockAddy = (struct sockaddr *) &sockAddy;

    //bind to the given port
    int bound = bind(sock, psockAddy, sizeof(sockAddy));
    
    if(bound == -1){
        return errorCheck("Bind Error");
    } else {
        printf("bind successful\n");
    }
    
    //wait for a message
    //int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
    char buffer[4096];
    struct sockaddr_storage outsideInfo;
    outsideInfo.ss_family = AF_INET;
    socklen_t outsideSize = sizeof(outsideInfo); 

    int recieved = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&outsideInfo, &outsideSize);

    if(recieved == -1){
        return errorCheck("Recieve Error");
    }else {
        printf("Successfully recieved %d bytes\n", recieved);
    }

    printf("sent message : %s\n", buffer);

    //condition check for the message
    char* returnMessage;

    if(strcmp(buffer, "ftp") == 0){
        returnMessage = "yes";
    } else {
        returnMessage = "no";
    }

    printf("message to return : %s\n", returnMessage);

    //sending message back
    int sent = sendto(sock, returnMessage, sizeof(returnMessage), 0, (struct sockaddr*)&outsideInfo, outsideSize);

    if(sent == -1){
        return errorCheck("Sendto Error");
    }else {
        printf("Successfully sent %d bytes\n", sent);
    }


    // Part 2

    //waiting for the packet
    int packetRecv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&outsideInfo, &outsideSize);

    if(packetRecv == -1){
        return errorCheck("Recieve Error");
    }else {
        printf("Successfully recieved packet of %d bytes\n", packetRecv);
    }

    // Print header fields
        unsigned int total_frag, frag_no, size;
        char filename[100];
        int header_offset = 0;

        memcpy(&total_frag, buffer + header_offset, sizeof(total_frag));
        header_offset += sizeof(total_frag) + 1;  // Skip ':'

        memcpy(&frag_no, buffer + header_offset, sizeof(frag_no));
        header_offset += sizeof(frag_no) + 1; // Skip ':'

        memcpy(&size, buffer + header_offset, sizeof(size));
        header_offset += sizeof(size) + 1; // Skip ':'

        strcpy(filename, buffer + header_offset);
        header_offset += strlen(filename) + 1 + 1;  // Skip filename and ':'

        printf("Total Fragments: %u\n", total_frag);
        printf("Fragment Number: %u\n", frag_no);
        printf("Size: %u\n", size);
        printf("Filename: %s\n", filename);

        // Print filedata in hexadecimal
        printf("Filedata:\n");
        for (int j = header_offset; j < (size + header_offset); ++j) {
            printf("%c ", (char)buffer[j]);
            if ((j - header_offset + 1) % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");

    //returning the appropriate acknowledgement
    returnMessage = "ack";
    int ackSent = sendto(sock, returnMessage, sizeof(returnMessage), 0, (struct sockaddr*)&outsideInfo, outsideSize);

    if(sent == -1){
        return errorCheck("AckSent Error");
    }else {
        printf("Successfully sent ack of %d bytes\n", ackSent);
    }

    //closing the socket
    int closeSock = close(sock);

    if(closeSock == -1){
        return errorCheck("Socket Close Error");
    } else { //REMOVE
        printf("Socket closed! -> %d\n", sock);
    }

    return 0;
}