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
// Functions to handle directory operations
#include<dirent.h>   


// Defining the number of max files that can be used
#define MAX_FILES 1024
// total number of clients that are currently present
int ttl_clients = 0;
// socket descriptors 
int sertxt_sd, sd_pdf_ser, cltn_sd;


void snd_txt_ser(char* command) {
    // Function to send a command to the text server

    // Uncomment to print the command being sent for debugging
    // printf("Sending command to text server: %s\n", command);

    // Send the command to the text server socket descriptor
    if(send(sertxt_sd, command, strlen(command), 0) < 0) {
        // If send() fails, print an error message to stderr
        fprintf(stderr, "send() failed\n");
        // Exit the program with status code 4
        exit(4);
    }
}

void psfd_sending(char* command) {
    // Function to send a command to the PDF server

    // Uncomment to print the command being sent for debugging
    // printf("Sending command to pdf server: %s\n", command);

    // Send the command to the PDF server socket descriptor
    if(send(sd_pdf_ser, command, strlen(command), 0) < 0) {
        // If send() fails, print an error message to stderr
        fprintf(stderr, "send() failure occurred\n");
        // Exit the program with status code 4
        exit(4);
    }
}

// bool type function for checking and creating directory
bool chk_creat_DIR_(const char *path) {
    // Buffer to store the full path of the directory
    char fullPath[1024];

    // Check if the path starts with "~/"
    if (strncmp(path, "~/", 2) == 0) {
        const char *homeDir = getenv("HOME");  // Get the HOME directory path
        snprintf(fullPath, sizeof(fullPath), "%s/%s", homeDir, path + 2);  // Skip the "~/"
    } else {
        // Path does not start with "~/", use the path as is
        snprintf(fullPath, sizeof(fullPath), "%s", path);  // Copy the path as-is
    }

    // Check if the directory exists
    struct stat st;
        // Use stat() to get information about the file or directory

    if (stat(fullPath, &st) == 0 && S_ISDIR(st.st_mode)) {
        // Directory exists
        printf("Directory '%s' already exists.\n", fullPath);
        // Return true indicating the directory exists
        return true;
    } else {
        // Directory does not exist, so attempt to create it
        // Use mkdir() to create the directory with permissions 0755
        if (mkdir(fullPath, 0755) == 0) {
            printf("Directory '%s' created successfully.\n", fullPath);
            return true;
        } else {
            // Error occurred while creating the directory
            // Print the error message from strerror() using errno
            printf("Error occurred while creating directory '%s': %s\n", fullPath, strerror(errno));
            // Return false indicating directory creation failed
            return false;
        }
    }
}



// Function to send a response message to the client over a given socket descriptor
void resp_to_ser(int cnnt_sd, char *rsp_msg) {
    // Send the response message to the client through the socket
    send(cnnt_sd, rsp_msg, strlen(rsp_msg), 0);
}

// Function to send the contents of a file to a client over a socket
void file_to_ser_sending(int cnnt_sd, const char *fi_name_0) {
    // Open the file in binary read mode
    FILE *fi_0 = fopen(fi_name_0, "rb");
    if (!fi_0) {
        // If the file could not be opened, log an error message and send an error response
        perror("Unable to open file for reading\n");
        resp_to_ser(cnnt_sd, "Unable to open file");
        return; // Exit the function if file opening fails
    }

    char bff_1[1024]; // Buffer to hold file data temporarily
    size_t bts_read_; // Number of bytes read from the file

    // Read the file in chunks and send each chunk to the client
    while ((bts_read_ = fread(bff_1, 1, sizeof(bff_1), fi_0)) > 0) {
        // Send the data read from the file to the client through the socket
        if (send(cnnt_sd, bff_1, bts_read_, 0) < 0) {
            // If sending data fails, log an error message, close the file, and exit
            perror("Failure occurred while sending the file data\n");
            fclose(fi_0);
            return;
        }
    }

    // Close the file after sending all data
    fclose(fi_0);

    // Notify the client that the file was sent successfully
    resp_to_ser(cnnt_sd, "File downloaded successfully");
}


void receive_file(int cnnt_sd, const char *fi_name_1) {
    // Open the file in binary write mode
    FILE *fi_001 = fopen(fi_name_1, "wb");
    if (!fi_001) {
        // If the file could not be created, log an error message and exit the program
        perror("Failure while creating the file to write\n");
        exit(1);
    }

    char buffer[1024]; // Buffer to hold incoming data
    ssize_t bts_rcvd; // Number of bytes received from the socket

    // Receive data from the socket into the buffer
    bts_rcvd = recv(cnnt_sd, buffer, sizeof(buffer), 0);
    if (bts_rcvd == -1) {
        // If receiving data fails, log an error message, close the file, and exit
        perror("Failure while receiving the file information\n");
        fclose(fi_001);
        exit(1);
    }

    // Write the received data to the file
    fwrite(buffer, 1, bts_rcvd, fi_001);

    // Close the file after writing data
    fclose(fi_001);

    // Notify the client that the file was stored successfully
    resp_to_ser(cnnt_sd, "File stored successfully");
}


// Function to check if the command contains "ufile" and a filename with the given extension
bool checkUfileInclusion(const char *command, const char *extension) {
    // Check if the command contains "ufile"
    char *u_pos = strstr(command, "ufile");

    // Check if the command contains a filename with the given extension
    char *ext_pos_for_ufile = strstr(command, extension);

    // Ensure "ufile" is present and the extension follows a space or is at the start
    if (u_pos != NULL && ext_pos_for_ufile != NULL) {
        // Check that the extension is part of a filename
        char *filename_start = ext_pos_for_ufile;
        while (filename_start > command && *(filename_start - 1) != ' ')
            filename_start--;

        // Check if the identified part ends with the given extension
        if (strncmp(ext_pos_for_ufile, extension, strlen(extension)) == 0) {
            return true;  // Both conditions met
        }
    }
    return false;  // Conditions not met
}

bool shouldCmdRouteOrnot(char* ufile_cmd){
    // Check if the command contains ".c" file extension
    if(checkUfileInclusion(ufile_cmd, ".c")){
        return false; // Do not route the command for ".c" files
    }

    // Check if the command contains ".txt" file extension
    if(checkUfileInclusion(ufile_cmd, ".txt")){
        // Define a buffer to receive data
        char ufile_cmd[10000];
        
        // Receive the ufile command from the client and send it to the txt server
        ssize_t bts_rcvd = recv(cltn_sd, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        snd_txt_ser(ufile_cmd); // Send the command to the txt server
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer

        // Receive the path command from the client and send it to the txt server
        bts_rcvd = recv(cltn_sd, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        snd_txt_ser(ufile_cmd); // Send the path command to the txt server
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer

        // Receive the flag response from the txt server and send it back to the client
        bts_rcvd = recv(sertxt_sd, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        resp_to_ser(cltn_sd, ufile_cmd); // Send the flag response back to the client
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer

        // Receive the filename command from the client and send it to the txt server
        bts_rcvd = recv(cltn_sd, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        snd_txt_ser(ufile_cmd); // Send the filename command to the txt server
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer

        // Receive the full file from the client and send it to the txt server
        bts_rcvd = recv(cltn_sd, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        snd_txt_ser(ufile_cmd); // Send the full file to the txt server
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer
        
        // Receive the success response from the txt server and send it back to the client
        bts_rcvd = recv(sertxt_sd, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        resp_to_ser(cltn_sd, ufile_cmd); // Send the success response back to the client
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer

    // Check if the command contains ".pdf" file extension
    } else if (checkUfileInclusion(ufile_cmd, ".pdf")){
        // Define a buffer to receive data
        char ufile_cmd[10000];
        
        // Receive the ufile command from the client and send it to the pdf server
        ssize_t bts_rcvd = recv(cltn_sd, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        psfd_sending(ufile_cmd); // Send the command to the pdf server
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer

        // Receive the path command from the client and send it to the pdf server
        bts_rcvd = recv(cltn_sd, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        psfd_sending(ufile_cmd); // Send the path command to the pdf server
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer

        // Receive the flag response from the pdf server and send it back to the client
        bts_rcvd = recv(sd_pdf_ser, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        resp_to_ser(cltn_sd, ufile_cmd); // Send the flag response back to the client
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer

        // Receive the filename command from the client and send it to the pdf server
        bts_rcvd = recv(cltn_sd, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        psfd_sending(ufile_cmd); // Send the filename command to the pdf server
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer

        // Receive the full file from the client and send it to the pdf server
        bts_rcvd = recv(cltn_sd, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        psfd_sending(ufile_cmd); // Send the full file to the pdf server
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer
        
        // Receive the success response from the pdf server and send it back to the client
        bts_rcvd = recv(sd_pdf_ser, ufile_cmd, sizeof(ufile_cmd) - 1, 0);
        ufile_cmd[bts_rcvd] = '\0'; // Null-terminate the received string
        resp_to_ser(cltn_sd, ufile_cmd); // Send the success response back to the client
        memset(ufile_cmd, 0, sizeof(ufile_cmd)); // Clear the buffer

    } else{
        return false; // Do not route the command if file extension is neither ".txt" nor ".pdf"
    }
    return false; // Default return value indicating failure to route the command
}


bool has_RM_andExt(const char *command, const char *extension) {
    // Check if the command string contains the substring "rmfile"
    char *rmfile_position = strstr(command, "rmfile");

    // Check if the command string contains the specified file extension
    char *ext_pos_for_ufile = strstr(command, extension);

    // Ensure both "rmfile" and the file extension are present in the command
    if (rmfile_position != NULL && ext_pos_for_ufile != NULL) {
        // Find the start of the filename by moving backwards from the extension position
        char *filename_start = ext_pos_for_ufile;
        while (filename_start > command && *(filename_start - 1) != ' ') {
            filename_start--; // Move backwards until a space is found or start of string is reached
        }

        // Check if the part identified by ext_pos_for_ufile actually ends with the given extension
        if (strncmp(ext_pos_for_ufile, extension, strlen(extension)) == 0) {
            return true;  // Both "rmfile" and the extension are correctly positioned
        }
    }
    return false;  // Either "rmfile" or the extension, or both, are not present or not correctly positioned
}


bool shouldHandleRmfileCommand(char* command) {
    // Check if the command contains "rmfile" with a ".c" extension
    if (has_RM_andExt(command, ".c")) {
        // printf("Command processed locally\n");
        char path[10000];
        
        // Extract the file path from the command string
        sscanf(command, "rmfile %s", path);

        // Attempt to delete the file locally
        if (remove(path) == 0) {
            // Send a success response to the client if the file was deleted successfully
            resp_to_ser(cltn_sd, "File deleted successfully.");
        } else {
            // Print error message and send a failure response to the client if the file could not be deleted
            perror("Failure while deletion op");
            resp_to_ser(cltn_sd, "Failure while deletion op");
        }

    // Check if the command contains "rmfile" with a ".txt" extension
    } else if (has_RM_andExt(command, ".txt")) {
        // printf("Command routed to txt server\n");
        char path[10000];
        
        // Receive the rmfile command from the client
        ssize_t bts_rcvd = recv(cltn_sd, path, sizeof(path) - 1, 0);
        path[bts_rcvd] = '\0'; // Null-terminate the received string
        snd_txt_ser(path); // Send the path to the txt server
        memset(path, 0, sizeof(path)); // Clear the buffer

        // Receive and send back the success response from the txt server
        bts_rcvd = recv(sertxt_sd, path, sizeof(path) - 1, 0);
        path[bts_rcvd] = '\0'; // Null-terminate the received string
        resp_to_ser(cltn_sd, path); // Send the response back to the client
        memset(path, 0, sizeof(path)); // Clear the buffer

    // Check if the command contains "rmfile" with a ".pdf" extension
    } else if (has_RM_andExt(command, ".pdf")) {
        // printf("Command routed to pdf server\n");
        char path[10000];
        
        // Receive the rmfile command from the client
        ssize_t bts_rcvd = recv(cltn_sd, path, sizeof(path) - 1, 0);
        path[bts_rcvd] = '\0'; // Null-terminate the received string
        psfd_sending(path); // Send the path to the pdf server
        memset(path, 0, sizeof(path)); // Clear the buffer

        // Receive and send back the success response from the pdf server
        bts_rcvd = recv(sd_pdf_ser, path, sizeof(path) - 1, 0);
        path[bts_rcvd] = '\0'; // Null-terminate the received string
        resp_to_ser(cltn_sd, path); // Send the response back to the client
        memset(path, 0, sizeof(path)); // Clear the buffer

    } else {
        return false;  // Return false if the command does not match any handled cases
    }
    return true;  // Return true if the command was successfully handled
}


void another_cmd_processing(char* command) {
    // Check if the command contains the substring "display"
    if (strstr(command, "display") != NULL) {
        // Tokenize the command string to extract the pathname
        char *token = strtok(command, " "); // Get the first token (command)
        token = strtok(NULL, " ");  // Get the second token (pathname)

        // Check if the pathname was successfully extracted
        if (token != NULL) {
            char pathname[1000];
            strcpy(pathname, token);  // Copy the pathname into a buffer

            // Call display_cmd_processing with the extracted pathname
            display_cmd_processing(cltn_sd, pathname);
        } else {
            // Send an error response if no pathname was provided
            resp_to_ser(cltn_sd, "Provide the file path");
        }
    }
}



// Function to process each client's requests
void prcss_clnt(int cnnt_sd, int current_client) {
    // Print a message indicating a new client connection
    printf("Connection was established successfully with the client %d\n", current_client);

    bool cnt_loop = true; // Flag to control the loop
    char command[1000]; // Buffer to store commands

    while (cnt_loop) {
        // Clear the command buffer
        memset(command, 0, sizeof(command));
        // Receive a command from the client
        ssize_t bts_rcvd = recv(cnnt_sd, command, sizeof(command) - 1, 0);

        // Check for errors or if the connection is closed
        if (bts_rcvd <= 0) {
            printf("Error occurred while receving the command or the client has closed the connection %d\n", current_client);
            cnt_loop = false; // Exit the loop
            break;
        }

        // Null-terminate the received data
        command[bts_rcvd] = '\0';
        // Print the received command
        printf("Received command from client %d: %s\n", current_client, command);

        // Process other types of commands
        another_cmd_processing(command);

        // Check if the command should be routed to another server
        if (shouldCmdRouteOrnot(command)) {
            // printf("Command Routed\n");
            break; // Exit the loop if the command is routed
        }

        char command_copy[1000];
        strcpy(command_copy, command); // Create a copy of the command for further processing

        char intr_cmd[1000];
        // Tokenize and process different commands
        if (strcmp(command, "ufile") == 0) {
            size_t bts_rcvd = recv(cnnt_sd, intr_cmd, sizeof(intr_cmd) - 1, 0);

            // Check for errors or if the connection is closed
            if (bts_rcvd <= 0) {
                printf("Error occurred while receving the command or the client has closed the connection %d\n", current_client);
                cnt_loop = false; // Exit the loop
                break;
            }
            intr_cmd[bts_rcvd] = '\0';

            // Check and create directory if needed
            if (chk_creat_DIR_(intr_cmd)) {
                char abs_path[10000];
                // Check if the path starts with "~/"
                if (strncmp(intr_cmd, "~/", 2) == 0) {
                    const char *homeDir = getenv("HOME"); // Get the HOME directory path
                    snprintf(abs_path, sizeof(abs_path), "%s/%s", homeDir, intr_cmd + 2); // Construct absolute path
                } else {
                    snprintf(abs_path, sizeof(abs_path), "%s", intr_cmd); // Use path as-is
                }

                char filePath[10000];
                strcpy(filePath, abs_path); // Copy the absolute path
                memset(intr_cmd, 0, sizeof(intr_cmd));
                resp_to_ser(cnnt_sd, "file_to_ser_sending"); // Notify client to send file

                // Receive file name
                size_t bts_rcvd = recv(cnnt_sd, intr_cmd, sizeof(intr_cmd) - 1, 0);
                if (bts_rcvd <= 0) {
                    printf("Error occurred while receving the command or the client has closed the connection %d\n", current_client);
                    cnt_loop = false; // Exit the loop
                    break;
                }
                intr_cmd[bts_rcvd] = '\0';
                char filename[10000];
                strcpy(filename, intr_cmd);

                char full_path[10000]; // Buffer for full path
                snprintf(full_path, sizeof(full_path), "%s/%s", abs_path, filename); // Construct full path
                receive_file(cnnt_sd, full_path); // Receive and save the file
            } else {
                // Notify client of failure to create directory
                resp_to_ser(cnnt_sd, "Failure occurred while creating the dir");
            }
        }

        if (strcmp(command, "dfile") == 0) {
            char filename[1000];
            sscanf(command, "dfile %s", filename); // Extract file name
            file_to_ser_sending(cnnt_sd, filename); // Send file to client
        }

        if (strcmp(command, "rmfile")) {
            // Add code to handle "rmfile" command here
        }

        // Clear the command buffer
        memset(command, 0, sizeof(command));
    }

    // Print a message when finished serving the client
    printf("Finished serving client %d, ending connection.\n", current_client);

    close(cnnt_sd); // Close the connection
    exit(0); // Exit the process
}

// Function to handle each client connection by forking a new process
void handleing_clnt_for_sever_side(int cnnt_sd, int current_client) {
    int pid = fork(); // Create a new process
    if (pid == 0) {
        prcss_clnt(cnnt_sd, current_client); // Child process: handle client
    } else if (pid == -1) {
        // Print an error if forking failed
        fprintf(stderr, "Failure occurred %d.\n", current_client);
    }
}

void req_from_clnt_(int cltn_sd) {
    char command[1000];
    // Receive the command from the client
    recv(cltn_sd, command, sizeof(command), 0);

    // Check if the command is for 'dfile'
    if (strncmp(command, "dfile", 5) == 0) {
        char *file_path = command + 6; // Skip "dfile " (5 chars + 1 space)

        // Open the file and send its contents
        FILE *file = fopen(file_path, "rb");
        if (file) {
            char buffer[1024];
            size_t bts_read_;
            while ((bts_read_ = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                if (send(cltn_sd, buffer, bts_read_, 0) == -1) {
                    // Print an error if sending fails
                    perror("unable to send the info\n");
                    break;
                }
            }
            fclose(file); // Close the file
        } else {
            // Print an error if file cannot be opened
            perror("File not opened\n");
        }
    }
}

void cret_nd_send_tar_fl(const char *filetype, int cltn_sd) {
    char cmd_tar[1024];
    char buffer[1024];
    FILE *fp;

    // Determine the tar file name based on file type
    char *fname_tar_file;
    if (strcmp(filetype, ".c") == 0) {
        fname_tar_file = "cfiles.tar";
    } else if (strcmp(filetype, ".txt") == 0) {
        fname_tar_file = "txtfiles.tar";
    } else if (strcmp(filetype, ".pdf") == 0) {
        fname_tar_file = "pdffiles.tar";
    } else {
        fprintf(stderr, "Please enter vaild file type\n");
        return;
    }

    // Create tar command
    snprintf(cmd_tar, sizeof(cmd_tar), "tar -cf %s -C ~/smain `find ~/smain -type f -name '*%s'`", fname_tar_file, filetype);

    // Execute the tar command
    if (system(cmd_tar) == -1) {
        perror("Failure: tar file not created");
        return;
    }

    // Send the tar file to the client
    FILE *tar_file = fopen(fname_tar_file, "rb");
    if (!tar_file) {
        perror("Failure: tar file not opened");
        return;
    }

    ssize_t bts_read_;
    while ((bts_read_ = fread(buffer, 1, sizeof(buffer), tar_file)) > 0) {
        if (send(cltn_sd, buffer, bts_read_, 0) == -1) {
            // Print an error if sending fails
            perror("Failure: tar file not sent");
            break;
        }
    }

    fclose(tar_file); // Close the tar file

    // Remove the tar file after sending
    remove(fname_tar_file);
}

void cnnt_ser(char *argv[]) {
    socklen_t len;
    // Define the required structure
    struct sockaddr_in server_add;

    // Create a socket at client side
    if ((sertxt_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // socket()
        fprintf(stderr, "Socket creation failure occurred\n");
        exit(1);
    }

    int num_prt_txt;
    // Build the structure for the socket
    server_add.sin_family = AF_INET; // Internet
    sscanf(argv[3], "%d", &num_prt_txt); // Get port number
    server_add.sin_port = htons((uint16_t)num_prt_txt); // Set port number

    // Convert address to binary format
    if (inet_pton(AF_INET, argv[2], &server_add.sin_addr) < 0) {
        fprintf(stderr, "inet_pton() has failed\n");
        exit(2);
    }

    // Connect to server, if failed, immediately exit
    if (connect(sertxt_sd, (struct sockaddr *) &server_add, sizeof(server_add)) < 0) { // connect()
        fprintf(stderr, "connect() failure occured and now exiting the program\n");
        exit(3);
    }
}

void cnnt_to_pdf_server(char *argv[]) {
    socklen_t len;
    // Define the required structure
    struct sockaddr_in server_add;

    // Create a socket at client side
    if ((sd_pdf_ser = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // socket()
        fprintf(stderr, "Socket was not created\n");
        exit(1);
    }

    int pdf_prt_num;
    // Build the structure for the socket
    server_add.sin_family = AF_INET; // Internet
    sscanf(argv[5], "%d", &pdf_prt_num); // Get port number
    server_add.sin_port = htons((uint16_t)pdf_prt_num); // Set port number

    // Convert address to binary format
    if (inet_pton(AF_INET, argv[4], &server_add.sin_addr) < 0) {
        fprintf(stderr, "inet_pton() has failed\n");
        exit(2);
    }

    // Connect to server, if failed, immediately exit
    if (connect(sd_pdf_ser, (struct sockaddr *) &server_add, sizeof(server_add)) < 0) { // connect()
        fprintf(stderr, "exiting the prpgram, connext() failed\n");
        exit(3);
    }
}

void expnd_Home(char *path, char *expandedPath, size_t size) {
    // Check if the path starts with '~'
    if (path[0] == '~') {
        const char *homeDir = getenv("HOME"); // Get the HOME directory path
        if (homeDir == NULL) {
            perror("Unable to get home directory");
            return;
        }

        // Construct the new path by replacing ~ with the home directory
        snprintf(expandedPath, size, "%s%s", homeDir, path + 1);
    } else {
        // If the path doesn't start with ~, just copy it as it is
        strncpy(expandedPath, path, size);
        expandedPath[size - 1] = '\0'; // Ensure null termination
    }
}

// Function to display files in a directory and return them in an array
char **dply_files_for_DIR(const char *path, int *fcont) {
    char abs_path[PATH_MAX];
    expnd_Home((char *)path, abs_path, sizeof(abs_path)); // Expand tilde to home directory

    struct dirent *rnt;
    DIR *dir = opendir(abs_path); // Open the directory

    if (dir == NULL) {
        // Print an error message if the directory couldn't be opened
        perror("Unable to open directory");
        return NULL;
    }

    // Allocate memory for the array of file names
    char **files = malloc(MAX_FILES * sizeof(char *));
    if (files == NULL) {
        perror("Unable to allocate memory for file list");
        closedir(dir); // Close the directory
        return NULL;
    }

    *fcont = 0; // Initialize file count
    while ((rnt = readdir(dir)) != NULL) {
        // Skip special entries "." and ".."
        if (strcmp(rnt->d_name, ".") != 0 && strcmp(rnt->d_name, "..") != 0) {
            // Check if the rnt is a regular file
            if (rnt->d_type == DT_REG) {
                files[*fcont] = strdup(rnt->d_name); // Duplicate the file name
                if (files[*fcont] == NULL) {
                    perror("Unable to allocate memory for file name");
                    // Free allocated memory before returning
                    for (int i = 0; i < *fcont; i++) {
                        free(files[i]);
                    }
                    free(files);
                    closedir(dir); // Close the directory
                    return NULL;
                }
                (*fcont)++; // Increment file count

                if (*fcont >= MAX_FILES) {
                    // Print a message if the maximum number of files is reached
                    printf("Reached maximum number of files that can be stored.\n");
                    break;
                }
            }
        }
    }

    closedir(dir); // Close the directory
    return files; // Return the array of file names
}

// Function to handle the 'display' command
void display_cmd_processing(int cnnt_sd, const char *pathname) {
    char list_fi[10000] = "";

    int fcont;
    char **files = dply_files_for_DIR(pathname, &fcont); // Get the list of files

    if (files != NULL) {
        printf("Files in directory:\n");
        for (int i = 0; i < fcont; i++) {
            printf("%s\n", files[i]);
            resp_to_ser(cnnt_sd, files[i]); // Send each file name to the client
            // free(files[i]);  // Uncomment to free each file name after use
        }
        free(files); // Free the array of file names
    }
}

// Function to handle receiving a response from the client
void got_the_resp(int cnnt_sd) {
    char command[10000];
    memset(command, 0, sizeof(command)); // Clear the command buffer
    ssize_t bts_rcvd = recv(cnnt_sd, command, sizeof(command) - 1, 0);

    // Check for errors or if the connection is closed
    if (bts_rcvd <= 0) {
        printf("Error occurred while receving the command or the client has closed the connection\n");
        return;
    }

    command[bts_rcvd] = '\0'; // Null-terminate the received data

    // Check if the command is for 'display'
    if (strncmp(command, "display", 7) == 0) {
        char pathname[10000];
        sscanf(command, "display %s", pathname); // Extract pathname
        display_cmd_processing(cnnt_sd, pathname); // Handle the display command
    } else {
        // Handle other commands (ufile, dfile, rmfile, etc.)
    }
}


int main(int argc, char *argv[]) {
    // Check if the number of command-line arguments is correct
    if (argc != 6) {
        // Print usage instructions if arguments are incorrect
        fprintf(stderr, "Usage: %s <Port#> <IP of Stxt> <Port# of Stxt> <IP of Spdf> <Port# of Spdf>\n", argv[0]);
        exit(0); // Exit the program with an error code
    }

    // Connect to the 'Stxt' and 'Spdf' servers using the IPs and ports provided in arguments
    cnnt_ser(argv); // Connect to the Stxt server
    cnnt_to_pdf_server(argv); // Connect to the Spdf server

    // Variables required for setting up and managing socket connections
    int lstn_sdesc, cnnt_sd, num_prt;
    struct sockaddr_in server_add;

    // Create a socket for listening to incoming connections
    if ((lstn_sdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // Print an error message if the socket could not be created
        fprintf(stderr, "Could not create socket\n");
        exit(1); // Exit the program with an error code
    }

    // Prepare the sockaddr_in structure for binding
    server_add.sin_family = AF_INET; // Set the address family to Internet
    server_add.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any IP address
    sscanf(argv[1], "%d", &num_prt); // Extract the port number from command-line arguments
    server_add.sin_port = htons((uint16_t)num_prt); // Set the port number for the socket

    // Bind the socket to the address and port number specified
    if (bind(lstn_sdesc, (struct sockaddr *) &server_add, sizeof(server_add)) < 0) {
        // Print an error message if binding fails
        fprintf(stderr, "Bind failed\n");
        close(lstn_sdesc); // Close the socket to release resources
        exit(1); // Exit the program with an error code
    }

    // Start listening for incoming connections
    if (listen(lstn_sdesc, 20) < 0) {
        // Print an error message if listening fails
        fprintf(stderr, "Listen failed\n");
        close(lstn_sdesc); // Close the socket to release resources
        exit(1); // Exit the program with an error code
    }

    // Print a message indicating the server is ready to accept connections
    printf("Server connection started, with the port number %d...\n", num_prt);

    while (1) {
        // Accept an incoming connection
        cnnt_sd = accept(lstn_sdesc, (struct sockaddr *) NULL, NULL);
        cltn_sd = cnnt_sd; // Set the global client socket descriptor

        // Check if the connection was successful
        if (cnnt_sd != -1) {
            ttl_clients++; // Increment the count of handled clients
            // Handle the client connection by forking a new process
            handleing_clnt_for_sever_side(cnnt_sd, ttl_clients);
        } else {
            // Print an error message if accepting the connection fails
            fprintf(stderr, "Accept() failure occurred\n");
        }
    }

    // Close the listening socket (though the loop above never exits)
    close(lstn_sdesc);
    return 0; // Return 0 to indicate successful completion (though the loop is infinite)
}



