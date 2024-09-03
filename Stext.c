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



// Function to send a response message to the client
void snd_resp(int cnnt_sd_txt, char *resp_msg_txt) {
    // Send the response message to the client using the connection socket descriptor
    // 'cnnt_sd_txt' is the socket descriptor for the client connection
    // 'resp_msg_txt' is the message to be sent
    // 'strlen(resp_msg_txt)' gives the length of the message to be sent
    // The '0' flag specifies default behavior for sending data
    send(cnnt_sd_txt, resp_msg_txt, strlen(resp_msg_txt), 0);
}


// Function to delete a file with the specified filename
bool dlt_file_txt(const char *txt_file) {
    // Attempt to remove the file specified by 'filename'
    // 'remove' returns 0 on success and a non-zero value on failure
    if (remove(txt_file) == 0) {
        // If remove is successful (returns 0), print a success message
        // '%s' will be replaced by the filename
        printf("File '%s' deleted successfully.\n", txt_file);
        // Return true to indicate that the file was deleted successfully
        return true;
    } else {
        // If remove fails (returns non-zero), print an error message
        // 'strerror(errno)' provides a description of the error
        // '%s' will be replaced by the filename
        // 'strerror(errno)' provides a string describing the error code stored in 'errno'
        printf("Error deleting file '%s': %s\n", txt_file, strerror(errno));
        // Return false to indicate that the file deletion failed
        return false;
    }
}


// Function to check if a directory exists and create it if it doesn't
bool chk_and_crat_dir(const char *path) {
    // Buffer to hold the full path after processing
    char fl__Path_[1024];

    // Get the HOME directory path from environment variables
    const char *homeDir_ = getenv("HOME");
    if (homeDir_ == NULL) {
        // If HOME directory is not found, print an error and return false
        printf("Error: HOME directory not found.\n");
        return false;
    }

    // Construct the full path by replacing "~/smain/" with "~/stext/"
    // 'path + 8' skips the "~/smain/" prefix to append the remaining path
    snprintf(fl__Path_, sizeof(fl__Path_), "%s/stext/%s", homeDir_, path + 8);

    // Structure to hold information about the file status
    struct stat st;

    // Check if the directory exists
    if (stat(fl__Path_, &st) == 0 && S_ISDIR(st.st_mode)) {
        // Directory exists
        printf("Directory '%s' already exists.\n", fl__Path_);
        return true;
    } else {
        // Directory does not exist, attempt to create it
        if (mkdir(fl__Path_, 0755) == 0) {
            // Directory created successfully
            printf("Directory '%s' created successfully.\n", fl__Path_);
            return true;
        } else {
            // Error occurred while creating the directory
            printf("Error creating directory '%s': %s\n", fl__Path_, strerror(errno));
            return false;
        }
    }
}


// Function to receive a file from a client and store it on the server
void rcd_file(int cnnt_sd_txt, const char *filename) {
    // Open the file for writing in binary mode
    FILE *file = fopen(filename, "wb");
    if (!file) {
        // If the file could not be created, print an error message and exit
        perror("Error: Could not create the file to write.\n");
        exit(1);
    }

    // Buffer to hold incoming data
    char buffer[1024];
    ssize_t bts_rcvd;

    // Receive data from the client
    bts_rcvd = recv(cnnt_sd_txt, buffer, sizeof(buffer), 0);
    if (bts_rcvd == -1) {
        // If receiving data fails, print an error message, close the file, and exit
        perror("Error: An issue occurred while receiving the file data.\n");
        fclose(file);
        exit(1);
    }

    // Write the received data to the file
    fwrite(buffer, 1, bts_rcvd, file);

    // Close the file after writing
    fclose(file);

    // Send a response back to the client indicating success
    snd_resp(cnnt_sd_txt, "File stored successfully");
}


// Function to receive commands from a client and handle them based on their type
char* rcvd_rsp(int cnnt_sd_txt) {
    bool cntt_loop = true;  // Flag to control the loop for receiving commands
    char cmd_txt[10000];        // Buffer to store the received command

    while (cntt_loop) {     // Loop to continuously receive and process commands
        // Clear the command buffer
        memset(cmd_txt, 0, sizeof(cmd_txt));

        // Receive the command from the client
        ssize_t bts_rcvd = recv(cnnt_sd_txt, cmd_txt, sizeof(cmd_txt) - 1, 0);

        if (bts_rcvd <= 0) {  // Check if there was an error or the connection was closed
            printf("Error receiving command or connection closed by client\n");
            cntt_loop = false;  // Exit the loop
            break;  // Break out of the loop
        }

        // Null-terminate the received data to ensure proper string handling
        cmd_txt[bts_rcvd] = '\0';

        // Create a copy of the command for processing
        char command_copy[1000];
        strcpy(command_copy, cmd_txt);

        // Buffer to store internal commands
        char intr_cmd_txt[1000];

        // Check if the command received is "ufile"
        if (strcmp(cmd_txt, "ufile") == 0) {
            // Existing ufile logic should be implemented here
            // For handling the "ufile" command, you would likely receive additional data
            // and process it accordingly (e.g., receiving and storing a file)
        } 
        // Check if the command received is "rmfile"
        else if (strcmp(cmd_txt, "rmfile") == 0) {
            // Receive additional data for "rmfile" command (filename to delete)
            size_t bts_rcvd = recv(cnnt_sd_txt, intr_cmd_txt, sizeof(intr_cmd_txt) - 1, 0);

            if (bts_rcvd <= 0) {  // Check if there was an error or the connection was closed
                printf("Error receiving command or connection closed by client\n");
                cntt_loop = false;  // Exit the loop
                break;  // Break out of the loop
            }

            // Null-terminate the received data to ensure proper string handling
            intr_cmd_txt[bts_rcvd] = '\0';

            // Prepare the absolute path for the file to delete
            char abs_txt_pathhh[10000];
            const char *homeDir_ = getenv("HOME");  // Get the HOME directory path
            snprintf(abs_txt_pathhh, sizeof(abs_txt_pathhh), "%s/stext/%s", homeDir_, intr_cmd_txt + 8);  
            // Construct the full path by appending the file name received to the HOME directory path

            // Attempt to delete the file
            if (dlt_file_txt(abs_txt_pathhh)) {
                // If file deletion is successful, send a success response
                snd_resp(cnnt_sd_txt, "File deleted successfully");
            } else {
                // If file deletion fails, send an error response
                snd_resp(cnnt_sd_txt, "Failed to delete file. Please try again.");
            }
        }
    }

    // Return null to indicate the function has completed processing (or could return a specific message)
    return NULL;
}



int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments are provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port#>\n", argv[0]);  // Print usage information
        exit(0);  // Exit the program
    }

    // Variables required for socket connection
    int lstn_sdesc, cnnt_sd_txt, port_number;
    struct sockaddr_in server_add;

    // Create a socket for communication
    if ((lstn_sdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Could not create socket\n");  // Print error message if socket creation fails
        exit(1);  // Exit the program with an error code
    }

    // Prepare the sockaddr_in structure
    server_add.sin_family = AF_INET;  // Set the address family to IPv4
    server_add.sin_addr.s_addr = htonl(INADDR_ANY);  // Set IP address to accept connections from any address
    sscanf(argv[1], "%d", &port_number);  // Convert port number from string to integer
    server_add.sin_port = htons((uint16_t)port_number);  // Set the port number for the server

    // Bind the socket to the address and port number
    if (bind(lstn_sdesc, (struct sockaddr *) &server_add, sizeof(server_add)) < 0) {
        fprintf(stderr, "Bind failed\n");  // Print error message if binding fails
        close(lstn_sdesc);  // Close the socket
        exit(1);  // Exit the program with an error code
    }

    // Listen for incoming connections
    if (listen(lstn_sdesc, 20) < 0) {
        fprintf(stderr, "Listen failed\n");  // Print error message if listening fails
        close(lstn_sdesc);  // Close the socket
        exit(1);  // Exit the program with an error code
    }

    // Print a message indicating that the server is started and listening on the specified port
    printf("Server started, listening on port %d...\n", port_number);

    // Accept an incoming connection
    cnnt_sd_txt = accept(lstn_sdesc, (struct sockaddr *) NULL, NULL);
    if (cnnt_sd_txt != -1) {
        // Print a message indicating that the connection was established
        printf("Connection Established.\n");

        // Continuously receive and process responses from the connected client
        while(1) {
            // Receive and print response from the client
            printf("%s\n", rcvd_rsp(cnnt_sd_txt));
        }
    } else {
        // Print an error message if the connection could not be accepted
        fprintf(stderr, "Accept failed\n");
    }

    // Close the listening socket (this line is never reached in the current infinite loop)
    close(lstn_sdesc);
    
    return 0;  // Return success code
}

