// Name: Mansi Pradipbhai Patel
// Student ID: 110135194
// Section: 3

// Standard Input/Output functions
#include<stdio.h>    
// General utilities like memory management
#include<stdlib.h>   
// Functions for creating and managing sockets
#include<sys/socket.h> 
// Data types used in system calls
#include<sys/types.h>  
// Functions for manipulating strings
#include<string.h>   
// Functions to handle date and time
#include<time.h>     
// Boolean data type and constants
#include<stdbool.h>  
// IP address manipulation and conversions
#include<arpa/inet.h> 
// Internet protocol family constants and structures
#include<netinet/in.h> 
// System call functions like read/write
#include<unistd.h>   
// File and directory attribute operations
#include<sys/stat.h> 
// Macros for error codes and handling
#include<errno.h>



void snd_rspn(int cnnt_sd, char *rsp_msg) {
    // Send the response message to the connected client
    // cnnt_sd: Socket descriptor for the connection with the client
    // rsp_msg: The message to be sent to the client

    // Send the message using the send() function
    // - cnnt_sd: The socket descriptor to use for sending the data
    // - rsp_msg: The buffer containing the message to be sent
    // - strlen(rsp_msg): The length of the message to be sent
    // - 0: Flags; 0 means no special flags
    if (send(cnnt_sd, rsp_msg, strlen(rsp_msg), 0) == -1) {
        // Check if sending the message failed
        perror("Failure while sending the response");  // Print an error message if sending fails
    }
}



bool chk_create_DIR(const char *path) {
    // Buffer to hold the full path of the directory to be checked or created
    char fl_path[1024];

    // Get the HOME directory path from the environment variables
    const char *hmDIR = getenv("HOME");
    // Check if getenv failed (i.e., HOME environment variable is not set)
    if (hmDIR == NULL) {
        fprintf(stderr, "Failure in setting the HOME env variable\n");
        return false;
    }

    // Construct the full path for the directory by replacing "~/smain/" with "~/spdf/"
    // path + 8 skips the "~/smain/" part of the path, leaving the rest of the path to be appended
    snprintf(fl_path, sizeof(fl_path), "%s/spdf/%s", hmDIR, path + 8);

    // Structure to hold the status information about the file or directory
    struct stat st;
    // Check if the directory exists and if it is a directory
    if (stat(fl_path, &st) == 0 && S_ISDIR(st.st_mode)) {
        // Directory exists
        printf("Directory '%s' already exists.\n", fl_path);
        return true;
    } else {
        // Directory does not exist, so attempt to create it
        if (mkdir(fl_path, 0755) == 0) {
            // Directory created successfully
            printf("Directory '%s' created successfully.\n", fl_path);
            return true;
        } else {
            // Error occurred while creating the directory
            printf("Failure while creating the directory'%s': %s\n", fl_path, strerror(errno));
            return false;
        }
    }
}



void rc_file(int cnnt_sd, const char *filename) {
    // Open a file for writing in binary mode
    FILE *file = fopen(filename, "wb");
    if (!file) {
        // Print an error message and exit if the file could not be created
        perror("Failure while writing in the file\n");
        exit(1);
    }

    // Buffer to store the incoming data
    char buffer[1024];
    // Variable to store the number of bytes received
    ssize_t buts_recvd;

    // Receive data from the connection
    buts_recvd = recv(cnnt_sd, buffer, sizeof(buffer), 0);

    // Check if receiving data was successful
    if (buts_recvd <= 0) {
        // Print an error message and exit if an error occurred during data reception
        perror("Failure occured while receving the file data\n");
        fclose(file);  // Close the file before exiting
        exit(1);
    }

    // Write the received data to the file
    fwrite(buffer, 1, buts_recvd, file);

    // Close the file after writing data
    fclose(file);

    // Send a response to the client indicating successful file storage
    snd_rspn(cnnt_sd, "File stored successfully");
}


bool dlt_the_file(const char *filename) {
    // Attempt to delete the file specified by 'filename'
    if (remove(filename) == 0) {
        // File was successfully deleted
        printf("File '%s' deleted successfully.\n", filename);
        return true;  // Return true to indicate success
    } else {
        // An error occurred while trying to delete the file
        // 'strerror(errno)' retrieves a human-readable error message
        printf("Failure while deleting the file '%s': %s\n", filename, strerror(errno));
        return false; // Return false to indicate failure
    }
}


char* rcv_rsp(int cnnt_sd) {
    bool cnt_loop = true;  // Flag to control the while loop
    char rscv_cmd[10000];        // Buffer to hold the received command

    while(cnt_loop) {
        // Clear the buffer to ensure no leftover data
        memset(rscv_cmd, 0, sizeof(rscv_cmd));

        // Receive data from the client socket into the command buffer
        ssize_t buts_recvd = recv(cnnt_sd, rscv_cmd, sizeof(rscv_cmd) - 1, 0);

        // Check if the recv call was successful
        if (buts_recvd <= 0) {
            // Print an error message if the connection was closed or an error occurred
            printf("Error receiving command or connection closed by client\n");
            cnt_loop = false;  // Exit the loop
            break;  // Exit the while loop
        }

        // Null-terminate the received data to safely use it as a string
        rscv_cmd[buts_recvd] = '\0';

        // Create a copy of the received command for further processing
        char command_copy[1000];
        strcpy(command_copy, rscv_cmd);

        // Buffer to hold the internal command
        char intr_Cmd[1000];

        // Check the received command and execute corresponding logic
        if (strcmp(rscv_cmd, "ufile") == 0) {
            // Logic for handling "ufile" command would go here
            // Currently, it's a placeholder for future implementation
        } else if (strcmp(rscv_cmd, "rmfile") == 0) {
            // For "rmfile", receive the path of the file to delete
            size_t buts_recvd = recv(cnnt_sd, intr_Cmd, sizeof(intr_Cmd) - 1, 0);

            // Check if the recv call was successful
            if (buts_recvd <= 0) {
                // Print an error message if the connection was closed or an error occurred
                printf("Error receiving command or connection closed by client\n");
                cnt_loop = false;  // Exit the loop
                break;  // Exit the while loop
            }

            // Null-terminate the received internal command data
            intr_Cmd[buts_recvd] = '\0';

            // Construct the absolute path to the file to be deleted
            char abs_paths[10000];
            const char *hmDIR = getenv("HOME");  // Get the HOME directory path
            snprintf(abs_paths, sizeof(abs_paths), "%s/spdf/%s", hmDIR, intr_Cmd + 8);  // Adjust path based on HOME directory

            // Attempt to delete the file and send a response based on success or failure
            if (dlt_the_file(abs_paths)) {
                snd_rspn(cnnt_sd, "File deleted successfully");
            } else {
                snd_rspn(cnnt_sd, "Failed to delete file. Please try again.");
            }
        }
    }

    // Return NULL or appropriate response based on the application logic
    // In this case, the function's return value is not used, but it is important to handle it properly
    return NULL;
}


int main(int argc, char *argv[]) {
    // Check if the correct number of arguments are provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port#>\n", argv[0]);
        exit(0);
    }

    // Variables required for socket connection
    int ltsn_sd, cnnt_sd, port_number;
    struct sockaddr_in server_add;

    // Create a socket using TCP/IP
    if ((ltsn_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // If socket creation fails, print an error message and exit
        fprintf(stderr, "Could not create socket\n");
        exit(1);
    }

    // Set up the sockaddr_in structure for the server address
    server_add.sin_family = AF_INET; // Address family (IPv4)
    server_add.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all network interfaces
    sscanf(argv[1], "%d", &port_number); // Convert port number from string to integer
    server_add.sin_port = htons((uint16_t)port_number); // Set port number (network byte order)

    // Bind the socket to the specified address and port
    if (bind(ltsn_sd, (struct sockaddr *) &server_add, sizeof(server_add)) < 0) {
        // If bind fails, print an error message and close the socket
        fprintf(stderr, "Bind failed\n");
        close(ltsn_sd);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(ltsn_sd, 20) < 0) {
        // If listening fails, print an error message and close the socket
        fprintf(stderr, "Listen failed\n");
        close(ltsn_sd);
        exit(1);
    }

    // Inform the user that the server is up and running
    printf("Server started, listening on port %d...\n", port_number);

    // Accept an incoming connection
    cnnt_sd = accept(ltsn_sd, (struct sockaddr *) NULL, NULL);
    if (cnnt_sd != -1) {
        // If connection is successful, print a message
        printf("Connection Established.\n");
        while (1) {
            // Continuously receive and print responses from the client
            printf("%s\n", rcv_rsp(cnnt_sd));
        }
    } else {
        // If connection fails, print an error message
        fprintf(stderr, "Accept failed\n");
    }

    // Close the listening socket (this line will not be reached due to infinite loop)
    close(ltsn_sd);

    return 0; // Return 0 to indicate successful completion
}

