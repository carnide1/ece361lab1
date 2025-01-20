#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) { 
        printf("Error - Port Number Required.\n"); 
        return 0;
    } else { 
        printf("Port Number: %s\n", argv[1]); 
    }    
    printf("Hello World! - server\n");

    //open a port at the number given by argv

    //wait for a message

    //condition check for the message

    return 0;
}