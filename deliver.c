#include <stdio.h>

int main(int argc, char *argv[]) {
    if( argc < 3){
        printf("Error - Both a Server Address and a Server Port Number are Required.\n");
        return 0;
    
    } else {
        printf("Server Address: %s\n", argv[1]);
        printf("Server Port Number: %s\n", argv[2]);
    }

    printf("Hello World! - deliver\n");

    return 0;
}