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
#include <time.h>

//for checking errors
int errorCheck(char * message){
    int err = errno;
    perror(message);
    return err;
}

//the struct for the packets
struct packet {
    unsigned int total_frag;
    unsigned int frag_no;
    unsigned int size;
    char* filename;
    char filedata[1000];
};

int main(int argc, char *argv[]) {
    //checking validity of the input based on number of inputs
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

    //checking if the file exists 
    if(access(filename, F_OK) == -1){
        return errorCheck("File does not exist");
    }

    //open a socket
    int sock = socket(AF_INET,  SOCK_DGRAM, 0);

    if(sock == -1){
        return errorCheck("Socket Open Error");
    } else { //REMOVE
        printf("Socket opened! -> %d\n", sock);
    }

    //creating the message to send
    char* returnMessage;
    returnMessage = "ftp";

    //setting the sockaddr type
    struct sockaddr_in sockAddy;
    memset(&sockAddy, 0, sizeof(sockAddy));
    sockAddy.sin_family = AF_INET;
    sockAddy.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &sockAddy.sin_addr);

    //getting the time before the message was sent
    struct timespec before;
    clock_gettime(CLOCK_MONOTONIC, &before);

    //sending the message
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

    //waiting for a message
    int recieved = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&outsideInfo, &outsideSize);

    if(recieved == -1){
        return errorCheck("Recieve Error");
    }else {
        printf("Successfully recieved %d bytes\n", recieved);
    }

    //getting the time it returned 
    struct timespec after;
    clock_gettime(CLOCK_MONOTONIC, &after);

    //outputting the RTT based on the differnce between the time sent and recieved
    double difference = (after.tv_sec - before.tv_sec) + (after.tv_nsec - before.tv_nsec) / 1e9;
    printf("2.1 - The measured round-trip time is %.9f seconds long\n", difference);

    //outputting relevant responses to output
    if (strcmp(buffer, "yes") == 0) {
        printf("A file transfer can start\n");
    } else {
        printf("A file transfer cannot start.\n");
        return 0;
    }


    // Part 2

    //determine the size of the file
    FILE *file = fopen(filename, "rb");
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    //determine the number of packets
    int packetNum = size / 1000 + 1;

    //creating the packets
    struct packet * packetList = (struct packet *)malloc(packetNum * (sizeof(struct packet)));
    long remainingBytes = size;

    for (int i = 0; i < packetNum; ++i) {
        packetList[i].total_frag = packetNum;
        packetList[i].frag_no = i + 1;
        packetList[i].filename = filename;

        long bytesExtracted = (remainingBytes < 1000) ? remainingBytes : 1000;
        packetList[i].size = bytesExtracted;
        
        fread(packetList[i].filedata, 1, bytesExtracted, file);

        remainingBytes -= bytesExtracted;
    }

    fclose(file);

    //sending all the packets
    for(int i = 0; i < packetNum; ++i){
        //convert packets to buffer
        long packetSize = sizeof(packetList[i].total_frag) + sizeof(packetList[i].frag_no) + strlen(packetList[i].filename) + 1 + sizeof(packetList[i].size) + (4*strlen(":")) + packetList[i].size;
        char packetBuffer[packetSize];

        // Copy header fields into the buffer
        int offset = 0;
        memcpy(packetBuffer + offset, &packetList[i].total_frag, sizeof(packetList[i].total_frag));
        offset += sizeof(packetList[i].total_frag);
        memcpy(packetBuffer + offset, ":", 1);
        offset += 1;
        memcpy(packetBuffer + offset, &packetList[i].frag_no, sizeof(packetList[i].frag_no));
        offset += sizeof(packetList[i].frag_no);
        memcpy(packetBuffer + offset, ":", 1);
        offset += 1;
        memcpy(packetBuffer + offset, &packetList[i].size, sizeof(packetList[i].size));
        offset += sizeof(packetList[i].size);
        memcpy(packetBuffer + offset, ":", 1);
        offset += 1;
        memcpy(packetBuffer + offset, packetList[i].filename, strlen(packetList[i].filename) + 1);
        offset += strlen(packetList[i].filename) + 1;
        memcpy(packetBuffer + offset, ":", 1);
        offset += 1;
        memcpy(packetBuffer + offset, packetList[i].filedata, packetList[i].size);
        offset += packetList[i].size;

        //send the packet buffer to the server
        int packetSent = sendto(sock, packetBuffer, packetSize, 0, (struct sockaddr*)&sockAddy, sizeof(sockAddy));

        if(sent == -1){
            return errorCheck("packetSend Error");
        }else {
            printf("Successfully sent packet of %d bytes\n", packetSent);
        }


        //wait for an acknowledgment from the server
        memset(buffer, 0, sizeof(buffer));
        int ackRecv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&outsideInfo, &outsideSize);

        if(recieved == -1){
            return errorCheck("Recieve Ack Error");
        }else {
            printf("Successfully recieved ack of %d bytes\n", ackRecv);
        }

        //check if the acknowledgment is ACK or NACK

        if (strcmp(buffer, "ack") == 0) {
            printf("Received Acknowledgement for packet: %s\n", buffer);
        } else {
            printf("Received No Acknowledgement for packet: %s\n", buffer);
            --i;
        }

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