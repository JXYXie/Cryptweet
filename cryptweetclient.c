/*************************************
 * cryptweet.c
 * 
 * Client file
 * 
 * Xin Yan (Jack) Xie
 * Some of the following methods are taken from Dr. Carey Williamson's website
 * https://pages.cpsc.ucalgary.ca/~carey/CPSC441/examples/wordlengthserver.c
 *************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

/// Defines
#define DEFAULTPORT 12346
#define MAX_LENGTH 1000

#define QUIT 0
#define ENTER 1

/*
 * Prompts the user for encode/decode mode
 */
void printmenu() {
    printf("\n");
    printf("Enter the following:\n");
    printf("  1 - Enter a message \n");
    printf("  0 - Exit.\n");
    printf("Selection: ");
}

/* 
 * Main method
 */
int main(int argc, char *argv[]) {
    
    int sockfd;
    struct sockaddr_in client;
    struct hostent *hp;
    char hostname[MAX_LENGTH];
    char message[MAX_LENGTH];
    char reply[MAX_LENGTH];
    char input;
    int port, len, bytes, choice;
    
    if (argc < 2) {
        port = DEFAULTPORT;
    } else {
        port = atoi(argv[1]);
    }
    
    /* Initialization of server sockaddr data structure */
    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    
    client.sin_addr.s_addr = inet_addr("136.159.5.25");
    
    // Create listening socket
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "CLIENT ERROR: socket() call failed!\n");
        exit(-1);
    }
    // Connect the socket
    if(connect(sockfd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "CLIENT ERROR: connect() call failed!\n");
        exit(-1);
    }
    
    // Get initial user choice
    printmenu();
    scanf("%d", &choice);
    // Main loop reading user input
    while (choice != QUIT) {
        
        if (choice == ENTER) {
            // Get user message
            input = getchar();
            printf("Enter the message: ");
            len = 0;
            while ((input = getchar()) != '\n') {
                message[len] = input;
                len++;
            }
            
            // Null terminates the message before sending it
            message[len] = '\0';
            // Sends message to the server
            send(sockfd , message, len, 0);
            memset(reply, '\0', 1000);
            // Get the response from the server
            if ((bytes = recv(sockfd, reply, 1000, 0)) > 0) {
                // Null terminates the reply
                //reply[bytes] = '\0';
                printf("Reply from server: ");
                printf("%s\n", reply);
            } else {
                fprintf(stderr, "ERROR: Server failed.\n");
                close(sockfd);
                exit (-1);
            }
        } else {
            // Try again
            printf("Invalid menu selection.\n");
        }
        printmenu;
        scanf("%d", &choice);
    }
    // Finished
    close(sockfd);
    exit(0);
    
}
