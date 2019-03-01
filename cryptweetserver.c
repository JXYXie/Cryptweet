/*************************************
 * cryptweet.c
 * 
 * Server file
 * 
 * Xin Yan (Jack) Xie
 * Some of the following methods are taken from Dr. Carey Williamson's website
 * https://pages.cpsc.ucalgary.ca/~carey/CPSC441/examples/wordlengthserver.c
 *************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>

#define DEFAULTPORT 12346
#define MAX_LENGTH 1000

int childsockfd;

// Hash map structure
typedef struct {
    char hash[MAX_LENGTH];         // Hash is the hexadecimal
    char key[MAX_LENGTH];          // Key is the decoded message
} hashmap;

/* 
 * This function is taken from Dr. Carey Williamson's TCP wordlengthserver.c file
 * Signal handler to do graceful exit if needed
 */
void handler (int signal) {
    close (childsockfd);
    exit(0);
}

/*
 * Takes decimal values and converts it to hexadecimal string
 */
char * hexer (int dec) {
    static char hex[10];
    sprintf(hex, "0x%04x", dec);
    return hex;
}

/* 
 * Sequential index hashing algorithm
 * Handles encoding and decoding
 */
char * sequential_index (char *key) {
    hashmap map[MAX_LENGTH];        // Create a hashmap of size MAX_LENGTH
    static char reply[MAX_LENGTH];  // Reply message
    char* pch;                      // Token
    int index = 1;                  // Index starts at 1
    int ptr = 0;                    // Initalize current hash pointer
    int len = 0;                    // Total number of words
    int mode;                       // Encode = 1, decode = 0, used to determine what hash attribute to use when returning
    
    // Split up the message into words
    pch = strtok(key, " !?,.;");
    // Iterate through tokens
    while (pch != NULL) {
        len++;
        // Is the word a hexadecimal?
        if ((pch[0] == '0') && (pch[1] == 'x')) {
            mode = 0;                                       // Set mode to decoding
            // If so check past hash values
            for (int i = 0; i < len; i++) {
                // Found same hash value
                if (strcmp(pch, map[i].hash) == 0) {
                    strcpy(map[ptr].hash, map[i].hash);     // Words that are the same should use same hexadecimal hash
                    strcpy(map[ptr].key, map[i].key);       // And should obviously have the same keys
                    printf("Found hash %s in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                    ptr++;                                  // Update new pointer location after insertion
                    break;                                  // Break out of search loop once found
                // Not found
                } else {
                    if (i == (len-1)) {                         // And exhausted map in search
                        strcpy(map[ptr].hash, hexer(index));    // Convert the index to hexadecimal and return it into the hash
                        strcpy(map[ptr].key, pch);              // Insert the new word
                        printf("Inserting hash %s in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                        index++;                                // Sequential index increments
                        ptr++;                                  // Update pointer
                    }
                }
            }
        // Word is not a hexadecimal
        } else {
            mode = 1;                                       // Set mode to encoding
            // Check past key (word) values
            for (int i = 0; i < len; i++) {
                // Found same key value
                if (strcmp(pch, map[i].key) == 0) {
                    strcpy(map[ptr].hash, map[i].hash);     // Words that are the same should use same hexadecimal hash
                    strcpy(map[ptr].key, map[i].key);       // And should obviously have the same keys
                    printf("Found key %s in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                    ptr++;                                  // Update new pointer location after insertion
                    break;                                  // Break out of search loop once found
                // Not found
                } else {
                    if (i == (len-1)) {                         // And exhausted map in search
                        strcpy(map[ptr].hash, hexer(index));    // Convert the index to hexadecimal and return it into the hash
                        strcpy(map[ptr].key, pch);              // Insert the new word
                        printf("Inserting key %s in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                        index++;                                // Sequential index increments
                        ptr++;                                  // Update pointer
                    }
                }
            }
        }
        // Next token
        pch = strtok(NULL, " !?,.;");
    }
    printf("There are %d words\n", len);
    // For loop to concatenate tokens together for reply
    for (int i = 0; i < len; i++) {
        // Decoding mode
        if (mode == 0) {
            strcat(reply, map[i].key);                      // Return the key values (words) since decoding from hexadecimal
            strcat(reply, " ");                             // add a space
        } else {
            strcat(reply, map[i].hash);                     // Return the hash values (hexadecimal) since encoding from words
            strcat(reply, " ");                             // add a space
        }
    }
    return reply;
}

/* 
 * Word sum hashing algorithm
 * Handles encoding and decoding
 */
char * word_sum (char *key) {
    hashmap map[MAX_LENGTH];        // Create a hashmap of size MAX_LENGTH
    static char reply[MAX_LENGTH];  // Reply message
    char* pch;                      // Token
    int ptr = 0;                    // Initalize current hash pointer
    int len = 0;                    // Total number of words
    int mode;                       // Encode = 1, decode = 0, used to determine what hash attribute to use when returning
    
    // Split up the message into words
    pch = strtok(key, " !?,.;");
    // Iterate through tokens
    while (pch != NULL) {
        len++;
        // Is the word a hexadecimal?
        if ((pch[0] == '0') && (pch[1] == 'x')) {
            mode = 0;                                       // Set mode to decoding
            // If so check past hash values
            for (int i = 0; i < len; i++) {
                // Found same hash value
                if (strcmp(pch, map[i].hash) == 0) {
                    strcpy(map[ptr].hash, map[i].hash);     // Words that are the same should use same hexadecimal hash
                    strcpy(map[ptr].key, map[i].key);       // And should obviously have the same keys
                    printf("Found hash <%s> in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                    ptr++;                                  // Update new pointer location after insertion
                    break;                                  // Break out of search loop once found
                // Not found
                } else {
                    if (i == (len-1)) {                         // And exhausted map in search
                        int wordsum = 0;                        // ASCII sum of word
                        for (int j = 0; j < (strlen(pch)); j++) {
                            wordsum = wordsum + pch[j];
                        }
                        strcpy(map[ptr].hash, hexer(wordsum));  // Convert the sum to hexadecimal and return it into the hash
                        strcpy(map[ptr].key, pch);              // Insert the new word
                        printf("Inserting hash <%s> in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                        ptr++;                                  // Update pointer
                    }
                }
            }
        // Word is not a hexadecimal
        } else {
            mode = 1;                                       // Set mode to encoding
            // Check past key (word) values
            for (int i = 0; i < len; i++) {
                // Found same key value
                if (strcmp(pch, map[i].key) == 0) {
                    strcpy(map[ptr].hash, map[i].hash);     // Words that are the same should use same hexadecimal hash
                    strcpy(map[ptr].key, map[i].key);       // And should obviously have the same keys
                    printf("Found key <%s> in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                    ptr++;                                  // Update new pointer location after insertion
                    break;                                  // Break out of search loop once found
                // Not found
                } else {
                    if (i == (len-1)) {                         // And exhausted map in search
                        int wordsum = 0;                        // ASCII sum of word
                        for (int j = 0; j < (strlen(pch)); j++) {
                            wordsum = wordsum + pch[j];
                        }
                        strcpy(map[ptr].hash, hexer(wordsum));  // Convert the sum to hexadecimal and return it into the hash
                        strcpy(map[ptr].key, pch);              // Insert the new word
                        printf("Inserting hash <%s> in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                        ptr++;                                  // Update pointer
                    }
                }
            }
        }
        // Next token
        pch = strtok(NULL, " !?,.;");
    }
    printf("There are %d words\n", len);
    // For loop to concatenate tokens together for reply
    for (int i = 0; i < len; i++) {
        // Decoding mode
        if (mode == 0) {
            strcat(reply, map[i].key);                      // Return the key values (words) since decoding from hexadecimal
            strcat(reply, " ");                             // add a space
        } else {
            strcat(reply, map[i].hash);                     // Return the hash values (hexadecimal) since encoding from words
            strcat(reply, " ");                             // add a space
        }
    }
    return reply;
}

/*
 * 16 bit internet checksum calculator
 * take from http://www.microhowto.info/howto/calculate_an_internet_protocol_checksum_in_c.html
 * and modified for this assignment
 */
unsigned short int checksumer(char* data, int length) {
    
    // Initialise the accumulator.
    unsigned long int acc=0xffff;
    
    // Handle complete 16-bit blocks.
    for (int i=0; i+1 < length; i+=2) {
        unsigned short int word;
        memcpy(&word, data+i, 2);
        acc += ntohs(word);
        if (acc > 0xffff) {
            acc -= 0xffff;
        }
    }

    // Handle any partial block at the end of the data.
    if (length & 1) {
        unsigned short int word=0;
        memcpy(&word, data + length-1, 1);
        acc += ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Return the checksum in network byte order.
    return (~acc);
}

/* 
 * Internet 16bit checksum hashing algorithm
 * Handles encoding and decoding
 */
char * check_sum (char *key) {
    hashmap map[MAX_LENGTH];        // Create a hashmap of size MAX_LENGTH
    static char reply[MAX_LENGTH];  // Reply message
    char* pch;                      // Token
    int ptr = 0;                    // Initalize current hash pointer
    int len = 0;                    // Total number of words
    int mode;                       // Encode = 1, decode = 0, used to determine what hash attribute to use when returning
    
    // Split up the message into words
    pch = strtok(key, " !?,.;");
    // Iterate through tokens
    while (pch != NULL) {
        len++;
        // Is the word a hexadecimal?
        if ((pch[0] == '0') && (pch[1] == 'x')) {
            mode = 0;                                       // Set mode to decoding
            // If so check past hash values
            for (int i = 0; i < len; i++) {
                // Found same hash value
                if (strcmp(pch, map[i].hash) == 0) {
                    strcpy(map[ptr].hash, map[i].hash);     // Words that are the same should use same hexadecimal hash
                    strcpy(map[ptr].key, map[i].key);       // And should obviously have the same keys
                    printf("Found hash <%s> in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                    ptr++;                                  // Update new pointer location after insertion
                    break;                                  // Break out of search loop once found
                // Not found
                } else {
                    if (i == (len-1)) {                         // And exhausted map in search
                        int wordsum = 0;                        // ASCII sum of word
                        // Get the 16 bit check sum
                        wordsum = checksumer(pch, strlen(pch));
                        
                        strcpy(map[ptr].hash, hexer(wordsum));  // Convert the sum to hexadecimal and return it into the hash
                        strcpy(map[ptr].key, pch);              // Insert the new word
                        printf("Inserting hash <%s> in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                        ptr++;                                  // Update pointer
                    }
                }
            }
        // Word is not a hexadecimal
        } else {
            mode = 1;                                       // Set mode to encoding
            // Check past key (word) values
            for (int i = 0; i < len; i++) {
                // Found same key value
                if (strcmp(pch, map[i].key) == 0) {
                    strcpy(map[ptr].hash, map[i].hash);     // Words that are the same should use same hexadecimal hash
                    strcpy(map[ptr].key, map[i].key);       // And should obviously have the same keys
                    printf("Found key <%s> in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                    ptr++;                                  // Update new pointer location after insertion
                    break;                                  // Break out of search loop once found
                // Not found
                } else {
                    if (i == (len-1)) {                         // And exhausted map in search
                        int wordsum = 0;                        // ASCII sum of word
                        // Get the 16 bit check sum
                        wordsum = checksumer(pch, strlen(pch));
                        
                        strcpy(map[ptr].hash, hexer(wordsum));  // Convert the sum to hexadecimal and return it into the hash
                        strcpy(map[ptr].key, pch);              // Insert the new word
                        printf("Inserting hash <%s> in position %d - key: <%s> hash: <%s>\n", pch, i, map[i].key, map[i].hash);
                        ptr++;                                  // Update pointer
                    }
                }
            }
        }
        // Next token
        pch = strtok(NULL, " !?,.;");
    }
    printf("There are %d words\n", len);
    // For loop to concatenate tokens together for reply
    for (int i = 0; i < len; i++) {
        // Decoding mode
        if (mode == 0) {
            strcat(reply, map[i].key);                      // Return the key values (words) since decoding from hexadecimal
            strcat(reply, " ");                             // add a space
        } else {
            strcat(reply, map[i].hash);                     // Return the hash values (hexadecimal) since encoding from words
            strcat(reply, " ");                             // add a space
        }
    }
    return reply;
}

/* 
 * Prompt for server admin to choose the type of hashing algorithm to use
 */
void hashingmenu() {
    printf("\n");
    printf("Enter the hashing algorithm the server should use:\n");
    printf("  1 - Sequential Index\n");
    printf("  2 - Word Sum\n");
    printf("  3 - Internet Checksum\n");
    printf("Hashing selection: ");
}

/*
 * main method
 * handles socket creation and connection
 */
int main(int argc, char *argv[]) {
    
    struct sockaddr_in server;
    static struct sigaction act;
    
    int parentsockfd;
    int pid, port;
    
    int hashtype;
    
    char message[MAX_LENGTH];
    char reply[MAX_LENGTH];
    
    // Get the port from the user
    if (argc < 2) {
        port = DEFAULTPORT;             // Default port if no arguments
    } else {
        port = atoi(argv[1]);
    }
    
    // Get server admin input for hashing algoithm to use
    hashingmenu();
    scanf("%d", &hashtype);
    
    /* Set up a signal handler to catch some weird termination conditions. */
    act.sa_handler = handler;
    sigfillset(&(act.sa_mask));
    sigaction(SIGPIPE, &act, NULL);
    
    /* Initialization of server sockaddr data structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // Create listening socket
    if((parentsockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "SERVER ERROR: socket() call failed!\n");
        exit(-1);
    }
    
    // Binds socket to address and port
    if (bind(parentsockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "SERVER ERROR: bind() call failed!\n");
        exit(-1);
    }
    
    // Listen
    if( listen(parentsockfd, 5) == -1 ) {
        fprintf(stderr, "SERVER ERROR: listen() call failed!\n");
        exit(-1);
    }
    
    // Initialize message strings just to be safe (null-terminated) */
    bzero(message, MAX_LENGTH);
    bzero(reply, MAX_LENGTH);
    
    // Main loop
    while (1) {
        
        // Accepts connection
        if ((childsockfd = accept(parentsockfd, NULL, NULL)) == -1) {
            fprintf(stderr, "SERVER ERROR: accept() call failed!\n");
            exit(-1);
        }
        
        // Create child process for client
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "SERVER ERROR: fork() call failed!\n");
            exit(-1);
        }
        // fork() return as 0 means child process
        else if (pid == 0) {
            // Close listener socket
            close(parentsockfd);
            // Get message from client
            while (recv(childsockfd, message, MAX_LENGTH, 0) > 0) {
                
                // Debug server message
                printf("SERVER MESSAGE: %s\n", message);
                
                // Run hashing algorithm depending on server settings
                if (hashtype == 1) {
                    strcpy(reply,sequential_index(message));
                } else if(hashtype == 2) {
                    strcpy(reply,word_sum(message));
                } else {
                    strcpy(reply,check_sum(message));
                }
                
                // Debug server reply
                printf("SERVER REPLY: %s\n", reply);
                
                // Send resulting message back to client
                send(childsockfd, reply, strlen(reply), 0);
                
                // Clear message buffers
                bzero(message, MAX_LENGTH);
                bzero(reply, MAX_LENGTH);
                
                // Close child socket and exit
                close(childsockfd);
                exit(0);
            }
        } else {
            // Create process
            fprintf(stderr, "Created child process %d to handle that client\n", pid);
            // Close child socket as parent process doesn't need it
            close(childsockfd);
        }
    }
    
}
