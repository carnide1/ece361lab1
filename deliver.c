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
    if( argc != 3){
        printf("Error - Both a Server Address and a Server Port Number are Required.\n");
        return 0;
    
    } else {
        printf("Server Address: %s\n", argv[1]);
        printf("Server Port Number: %s\n", argv[2]);
    }
    
    //1. Ask the user to input a message as follows:
    // ftp <file name>
    char command[4096];
    char filename[4096];

    printf("Input a message as follows: ftp <file name>\n");
    scanf("%s %s", command, filename); 

    if (strcmp(command, "ftp") == 0) {
        printf("Received FTP command for file: %s\n", filename);
    } else {
        printf("Invalid command received.\n");
        return 0;
    }

    // 2. Check the existence of the file:
    // a. if exist, send a message “ftp” to the server
    // b. else, exit

    if(access(filename, F_OK | W_OK | X_OK) == -1){
        return errorCheck("File does not exist");
    }
    
    //open a socket
    int sock = socket(AF_INET,  SOCK_DGRAM, 0);

    if(sock == -1){
        return errorCheck("Socket Open Error");
    } else { //REMOVE
        printf("Socket opened! -> %d\n", sock);
    }

    char* returnMessage;
    returnMessage = "ftp";

    //setting the sockaddr type
    struct sockaddr_in sockAddy;
    memset(&sockAddy, 0, sizeof(sockAddy));
    sockAddy.sin_family = AF_INET;
    sockAddy.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &sockAddy.sin_addr);

    int sent = sendto(sock, returnMessage, strlen(returnMessage), 0, (struct sockaddr*)&sockAddy, sizeof(sockAddy));

    if(sent == -1){
        return errorCheck("Sendto Error");
    }else {
        printf("Successfully sent %d bytes\n", sent);
    }


    // 3. Receive a message from the server:
    // a. if the message is “yes”, print out “A file transfer can start.”
    // b. else, exit
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

    if (strcmp(buffer, "yes") == 0) {
        printf("A file transfer can start\n");
    } else {
        printf("A file transfer cannot start.\n");
        return 0;
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