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

//the struct for the packets
struct packet {
    unsigned int total_frag;
    unsigned int frag_no;
    unsigned int size;
    char* filename;
    char filedata[1000];
};

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
    unsigned int total_frag, frag_no, size;
    char filename[100];
    int packetNum = 4096;
    struct packet *packetList = (struct packet *)malloc(packetNum * (sizeof(struct packet)));

    do {
        //cleaning out buffer
        memset(buffer, 0, sizeof(buffer));

        //getting the packet from the client
        int packetRecv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&outsideInfo, &outsideSize);

        if(packetRecv == -1){
            return errorCheck("Recieve Error");
        }else {
            printf("Successfully recieved packet of %d bytes\n", packetRecv);
        }

        // get header fields
        int header_offset = 0;

        memcpy(&total_frag, buffer + header_offset, sizeof(total_frag));
        header_offset += sizeof(total_frag) + 1;
        memcpy(&frag_no, buffer + header_offset, sizeof(frag_no));
        header_offset += sizeof(frag_no) + 1;
        memcpy(&size, buffer + header_offset, sizeof(size));
        header_offset += sizeof(size) + 1; 
        strcpy(filename, buffer + header_offset);
        header_offset += strlen(filename) + 1 + 1; 

        //making sure the array allocated earlier works
        if(total_frag > packetNum){
            struct packet * temp = packetList;
            packetNum = total_frag;
            packetList = (struct packet *)malloc(packetNum * (sizeof(struct packet)));
            free(temp);
        }

        //populating the array with the sent packet
        packetList[frag_no-1].total_frag = total_frag;
        packetList[frag_no-1].frag_no = frag_no;
        packetList[frag_no-1].size = size;
        packetList[frag_no-1].filename = filename;
        memcpy(packetList[frag_no-1].filedata, buffer + header_offset, size);

        //returning the appropriate acknowledgement
        returnMessage = "ack";
        int ackSent = sendto(sock, returnMessage, sizeof(returnMessage), 0, (struct sockaddr*)&outsideInfo, outsideSize);

        if(sent == -1){
            return errorCheck("AckSent Error");
        }else {
            printf("Successfully sent ack of %d bytes\n", ackSent);
        }

    }while(total_frag != frag_no);

    //we've now finished reading all the packets, so we now save it as a file

    char newFilename[4096] = "deliver";
    strcat(newFilename, filename);

    // create the file
    FILE *file = fopen(newFilename, "w");
    if (file == NULL) {
        return errorCheck("File open error");
    }
    
    for(int i = 0; i < total_frag; ++i){
        fwrite(packetList[i].filedata, sizeof(char), packetList[i].size, file);
    }    

    fclose(file);


    //closing the socket
    int closeSock = close(sock);

    if(closeSock == -1){
        return errorCheck("Socket Close Error");
    } else { //REMOVE
        printf("Socket closed! -> %d\n", sock);
    }

    return 0;
}