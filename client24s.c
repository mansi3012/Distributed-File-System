// Name: Mansi Pradipbhai Patel
// Student ID: 110135194
// Section: 3

// Required header for socket structures
#include <netinet/in.h>  
// Basic I/O operations
#include <stdio.h>       
// Socket creation and handling
#include <sys/socket.h>  
#include <sys/types.h>   
// Network structure to binary conversion
#include <arpa/inet.h>   
// String manipulation functions
#include <string.h>      
// POSIX system calls
#include <unistd.h>      
// Boolean type definition
#include <stdbool.h>     

// User input storage
char cmdArrInput[1000];  
// First word of the command
char firstWord[100];    
// Client socket descriptor
int clt_sdesc;          
// Port number for connection
int prt_num;        



// Function to send the command entered by the user to the server
void cmd_to_server(int clt_sdesc, bool msg_flag, char *msg) {
    // Check if sending the message is enabled
    if (msg_flag == true) {
        // Attempt to send the message to the server
        // 'clt_sdesc' is the socket descriptor
        // 'message' is the string to be sent
        // 'strlen(message)' gives the ln_1 of the message to be sent
        // '0' specifies no special flags
        if (send(clt_sdesc, msg, strlen(msg), 0) < 0) {
            // If 'send' fails, print an error message to stderr
            // Exit the program with status code 4 to indicate a failure in sending data
            fprintf(stderr, "Send() failure occured\n");
            exit(4);
        }
    }
}



// Function to process the response from the server
bool prcs_spec_cmd(int clt_sdesc, char *msg_to_compare) {
    // Buffer to store the server's response
    char buff[50000];
    // Initialize the buffer with null bytes
    memset(buff, 0, sizeof(buff));

    // Receive the response from the server
    // 'clt_sdesc' is the socket descriptor
    // 'buffer' is where the response will be stored
    // '50000' is the maximum number of bytes to read
    // '0' specifies no special flags
    recv(clt_sdesc, buff, 50000, 0);

    // Compare the received response with the expected message
    // 'strcmp' returns 0 if the strings are equal
    if (strcmp(buff, msg_to_compare) == 0) {
        // If the response matches the expected message, return true
        return true;
    }

    // Print the server's response to the console
    printf("Server Responded: %s\n", buff);

    // If the response does not match, return false
    return false;
}


// Function to send a file to the client
void send_file(int clt_sdesc, char* path_of_file) {
    // Open the file in binary read mode
    FILE *file = fopen(path_of_file, "rb");
    if (!file) {
        // Error opening the file
        perror("Failure: the requested file was not opened\n");
        exit(1);
    }

    // Move the file pointer to the end to get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    // Move the file pointer back to the beginning
    fseek(file, 0, SEEK_SET);

    // Dynamically allocate memory to store the file data
    char *file_data = (char *)malloc(file_size);
    if (file_data == NULL) {
        // Error allocating memory
        perror("Failure: Memory allocation\n");
        fclose(file);
        exit(1);
    }

    // Read the file contents into the allocated memory
    fread(file_data, 1, file_size, file);
    // Close the file after reading
    fclose(file);

    // Send the file data to the client
    ssize_t bytes_sent = send(clt_sdesc, file_data, file_size, 0);
    if (bytes_sent == -1) {
        // Error sending the file data
        fprintf(stderr, "Failure: while sending the information\n");
        free(file_data);  // Free allocated memory before exiting
        exit(1);
    }

    // Free the dynamically allocated memory
    free(file_data);
}


// Function to process the response received from the server
void process_response(int clt_sdesc) {
    // Notify that the server response is being awaited
    printf("Waiting for the server to respond...\n");

    // Buffer to store incoming data from the server
    char buffer[5000];

    // Loop to handle receiving data
    for (int i = 0; i < sizeof(buffer); i++) {
        // Initialize the buffer with null bytes
        memset(buffer, 0, sizeof(buffer));

        // Receive data from the server into the buffer
        int bytes_rcd = recv(clt_sdesc, buffer, sizeof(buffer) - 1, 0);

        // Check if data was received or if there was an error
        if (bytes_rcd <= 0) {
            // Exit loop if no data is received or an error occurs
            break;
        }

        // Null-terminate the received data to ensure proper string handling
        buffer[bytes_rcd] = '\0';

        // Print the server response
        printf("%s\n", buffer);
    }
}


// Function to count the number of arguments in the input
int countArguments(char *input) {
    int count = 0; // Initialize argument count to zero
    // Tokenize the input string based on spaces
    char *tkn = strtok(input, " ");
    
    // Continue tokenizing until no more tokens are found
    while (tkn != NULL) {
        count++; // Increment count for each token found
        tkn = strtok(NULL, " "); // Get the next token
    }
    return count; // Return the total number of arguments
}

// Function to check if a file exists in a given path
bool fileExists(const char *filepath) {
    FILE *file;

    // Attempt to open the file in read mode
    file = fopen(filepath, "r");  
    if (file) {
        fclose(file);  // Close the file if it was successfully opened
        return true;   // Return true indicating the file exists
    } else {
        return false;  // Return false indicating the file does not exist
    }
}


// Function to handle the 'ufile' command
void ufile_cmd_process(char *filename, char *destination_path) {
    // Check if the file exists in the client's current working directory
    if (!fileExists(filename)) {
        printf("Error: The file %s does not exist.\n", filename);
        return; // Exit function if file does not exist
    }

    // Determine the file type by checking the file extension
    char *ext = strrchr(filename, '.'); // Find the last occurrence of '.' in the filename
    if (ext == NULL || 
        (strcmp(ext, ".c") != 0 && strcmp(ext, ".txt") != 0 && strcmp(ext, ".pdf") != 0)) {
        // Check if the file extension is valid
        printf("Error occurred: Invalid file type. Only .c, .txt, and .pdf files are supported.\n");
        return; // Exit function if file type is unsupported
    }

    char new_path[1000]; // Buffer to store the new path based on file type
    // Construct the new path based on the file type
    if (strcmp(ext, ".c") == 0) {
        // For .c files, (code commented out for transfer details)
        // printf("Transferring %s to %s on Smain server...\n", filename, new_path);
    } else if (strcmp(ext, ".txt") == 0) {
        snprintf(new_path, sizeof(new_path), "%s", destination_path); // Copy destination_path
        strncpy(new_path, "~stext", 7); // Replace initial part with "~stext"
        // printf("Transferring %s to %s on Stext server...\n", filename, new_path);
    } else if (strcmp(ext, ".pdf") == 0) {
        snprintf(new_path, sizeof(new_path), "%s", destination_path); // Copy destination_path
        strncpy(new_path, "~spdf", 7); // Replace initial part with "~spdf"
        // printf("Transferring %s to %s on Spdf server...\n", filename, new_path);
    } else {
        // In case of an unsupported file type (should not be reached)
        printf("Error: Unsupported file type.\n");
        return; // Exit function if file type is unsupported
    }

    // Calculate the ln_1 required for the combined command string
    size_t ln_1 = strlen("ufile") + strlen(filename) + strlen(destination_path) + 3;  // 3 for two spaces and null terminator

    // Allocate memory for the combined command string
    char *cmbCmd = (char *)malloc(ln_1 * sizeof(char));
    if (cmbCmd == NULL) {
        // Handle memory allocation failure
        printf("Memory allocation failed\n");
        return; // Exit function if memory allocation fails
    }

    // Combine the command flag, filename, and destination path into a single command string
    snprintf(cmbCmd, ln_1, "ufile %s %s", filename, destination_path);

    // Send the combined command to the server
    cmd_to_server(clt_sdesc, true, cmbCmd);
    // Free the allocated memory for the command string
    free(cmbCmd);

    // Pause briefly before sending further commands
    sleep(1);
    // Send 'ufile' command to the server again
    cmd_to_server(clt_sdesc, true, "ufile");
    sleep(1);
    // Send the destination path to the server
    cmd_to_server(clt_sdesc, true, destination_path);

    // Pause briefly before checking server response
    sleep(2);
    // Check for server response indicating file transfer should start
    if (prcs_spec_cmd(clt_sdesc, "SEND_FILE")) {
        // Send the filename to the server to indicate which file is being sent
        cmd_to_server(clt_sdesc, true, filename);
        // Send the actual file data
        send_file(clt_sdesc, filename);
        // Process server's final response
        process_response(clt_sdesc);
    } else {
        // If the server response is not as expected, exit function
        return;
    }
}


// Function to receive a file from the server and save it locally
void rec_file_from_server(int server_sd, const char *fname) {
    // Open the file for writing in binary mode
    FILE *fi = fopen(fname, "wb");
    if (!fi) {
        // Print an error message if the file could not be created
        perror("Failure while creating the file\n");
        return; // Exit function if file opening fails
    }

    char bff1[1024]; // Buffer to store received data
    ssize_t bytes_rcd; // Variable to store the number of bytes received

    // Loop to receive data from the server
    while ((bytes_rcd = recv(server_sd, bff1, sizeof(bff1), 0)) > 0) {
        // Write received data to the file
        size_t byts_wrtn = fwrite(bff1, 1, bytes_rcd, fi);
        if (byts_wrtn < bytes_rcd) {
            // Print an error message if not all data was written
            perror("Failure while writing in the file\n");
            break; // Exit loop if writing fails
        }
    }

    if (bytes_rcd < 0) {
        // Print an error message if receiving data fails
        perror("Failure while receiving the file info\n");
    } else {
        // Print a success message if file received successfully
        printf("File '%s' downloaded successfully.\n", fname);
    }

    // Close the file
    fclose(file);
}



// Function to handle the 'dfile' command, requesting a file from the server
void handleDfileCommand(int server_sd, char *fpath) {
    char cmd1[1000]; // Buffer to store the command

    // Construct the 'dfile' command with the file path
    snprintf(cmd1, sizeof(cmd1), "dfile %s", fpath);

    // Send the 'dfile' command to the server
    if (send(server_sd, cmd1, strlen(cmd1), 0) < 0) {
        // Print an error message if sending the command fails
        perror("Failure while sending the file\n");
        return; // Exit function if sending fails
    }

    // Extract the filename from the file path
    char *fname = strrchr(fpath, '/');
    fname = (fname) ? fname + 1 : fpath; // Get filename or use full path if '/' is not found

    // Receive the file from the server and save it locally
    rec_file_from_server(server_sd, fname);
}



// Function to handle the 'rmfile' command, deleting a file from the server
void rmfile_handle_removal(char *fname) {
    // Retrieve the home directory path
    char *hmDir = getenv("HOME");
    char full_dest_path[1024]; // Buffer to store the full path to the file

    // Construct the full path to the file by replacing "~smain" with the actual path
    snprintf(full_dest_path, sizeof(full_dest_path), "%s/smain/%s", hmDir, fname + 7); // Skip the "~smain" part

    // Attempt to delete the file
    if (remove(full_dest_path) == 0) {
        // Print success message if file is deleted
        printf("File '%s' deleted successfully.\n", full_dest_path);
    } else {
        // Print error message if file deletion fails
        perror("Failure while deleting the file");
    }
}


// Function to handle the 'display' command
void display_handling_of_the_commands(char *pathname) {
    // Define the command flag
    char *cmdflg = "display";
    // Calculate the ln_1 required for the combined command string
    size_t ln_1 = strlen(cmdflg) + strlen(pathname) + 2;  // 1 for space and 1 for null terminator

    // Allocate memory for the combined command string
    char *cmbCmd = (char *)malloc(ln_1 * sizeof(char));
    if (cmbCmd == NULL) {
        // Handle memory allocation failure
        printf("Memory allocation is failed\n");
        return;
    }

    // Combine the command flag and pathname into a single string
    snprintf(cmbCmd, ln_1, "%s %s", cmdflg, pathname);

    // Send the combined command to the server
    cmd_to_server(clt_sdesc, true, cmbCmd);

    // Free the allocated memory for the command string
    free(cmbCmd);

    // Receive and display the list of files from the server
    process_response(clt_sdesc);
}


// Function to handle the 'dtar' command
void dtar_handling_cmd(int server_sd, char *ftyp) {
    // Create the command string with 'dtar' and the specified filetype
    char cmd2[1000];
    snprintf(cmd2, sizeof(cmd2), "dtar %s", ftyp);

    // Send the 'dtar' command to the server
    if (send(server_sd, cmd2, strlen(cmd2), 0) < 0) {
        // Handle error in sending the command
        perror("Failure while sending the command to the server");
        return;
    }

    // Receive the tar file from the server and save it with a fixed name
    rec_file_from_server(server_sd, "received_tar_file.tar");
}


int main(int argc, char *argv[]) {
    // Define variables for socket operations and server address
    socklen_t len; // Variable to store the ln_1 of the address structure
    struct sockaddr_in server_add; // Structure to hold server address information

    // Check if the number of command-line arguments is correct
    if (argc != 3) {
        // Print usage instructions if the number of arguments is incorrect
        printf("Please check structure: %s <IP> <Port#>\n", argv[0]);
        exit(0); // Exit the program with status 0 (success)
    }

    // Create a socket for communication
    if ((clt_sdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // Create a TCP socket
        // Print error message if socket creation fails
        fprintf(stderr, "Unabele to create the socket\n");
        exit(1); // Exit the program with status 1 (error)
    }

    // Initialize the server address structure
    server_add.sin_family = AF_INET; // Use the Internet address family
    sscanf(argv[2], "%d", &prt_num); // Convert port number from string to integer
    server_add.sin_port = htons((uint16_t)prt_num); // Convert port number to network byte order

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, argv[1], &server_add.sin_addr) < 0) { // Convert IP address
        // Print error message if address conversion fails
        fprintf(stderr, "inet_pton() has failed\n");
        exit(2); // Exit the program with status 2 (error)
    }

    // Connect to the server using the specified address
    if (connect(clt_sdesc, (struct sockaddr *) &server_add, sizeof(server_add)) < 0) { // Connect to server
        // Print error message if connection fails
        fprintf(stderr, "Connect() failed and now exiting\n");
        exit(3); // Exit the program with status 3 (error)
    }

    // Main loop to continuously take user commands
    for (;;) {
        printf("\nEnter the command: "); // Prompt user for a command

        // Read user input from stdin and store it in cmdArrInput
        fgets(cmdArrInput, sizeof(cmdArrInput), stdin);
        cmdArrInput[strcspn(cmdArrInput, "\n")] = '\0'; // Remove newline character

        // Extract the first word (command) from the input
        sscanf(cmdArrInput, "%s", firstWord);
        char cmdBuffer[1000];
        strcpy(cmdBuffer, cmdArrInput); // Copy input for argument counting

        // Count the number of arguments in the command
        int argCount = countArguments(cmdBuffer);

        // Prepare to parse the arguments from the command input
        char tkn_buff[1000];
        strcpy(tkn_buff, cmdArrInput); // Copy input for tokenization
        char *args[4]; // Array to store up to 3 arguments plus NULL terminator
        int i = 0;

        // Tokenize the command input to extract arguments
        char *tkn = strtok(tkn_buff, " ");
        while (tkn != NULL && i < 3) { // Extract up to 3 tokens
            args[i++] = tkn; // Store each token in the args array
            tkn = strtok(NULL, " "); // Get the next token
        }
        args[i] = NULL; // Null-terminate the argument array

        // Handle the command based on the first word
        if (strcmp(firstWord, "ufile") == 0) { // Check for 'ufile' command
            if (argCount == 3) { // Verify that the correct number of arguments are provided
                char *f_Namee = args[1]; // Get the filename argument
                char *dst_f_path = args[2]; // Get the destination path argument
                printf("The word is: %s with %d arguments\n", firstWord, argCount);
                ufile_cmd_process(f_Namee, dst_f_path); // Handle the 'ufile' command
            } else {
                // Print error message if argument count is incorrect
                printf("'ufile' command requires exactly 3 arguments\n");
            }
        } else if (strcmp(firstWord, "dfile") == 0) { // Check for 'dfile' command
            if (argCount == 2) { // Verify that the correct number of arguments are provided
                char *fpath_ = args[1]; // Get the file path argument
                printf("The word is: %s with %d arguments\n", firstWord, argCount);
                handleDfileCommand(clt_sdesc, fpath_); // Handle the 'dfile' command
            } else {
                // Print error message if argument count is incorrect
                printf("'dfile' command requires exactly 2 arguments\n");
            }
        } else if (strcmp(firstWord, "rmfile") == 0) { // Check for 'rmfile' command
            if (argCount == 2) { // Verify that the correct number of arguments are provided
                printf("The word is: %s with %d arguments\n", firstWord, argCount);
                rmfile_handle_removal(args[1]); // Handle the 'rmfile' command
            } else {
                // Print error message if argument count is incorrect
                printf("'rmfile' command requires exactly 2 arguments\n");
            }
        } else if (strcmp(firstWord, "dtar") == 0) { // Check for 'dtar' command
            if (argCount == 2) { // Verify that the correct number of arguments are provided
                char *filetype = args[1]; // Get the file type argument
                printf("The word is: %s with %d arguments\n", firstWord, argCount);
                dtar_handling_cmd(clt_sdesc, filetype); // Handle the 'dtar' command
            } else {
                // Print error message if argument count is incorrect
                printf("'dtar' command requires exactly 2 arguments\n");
            }
        } else if (strcmp(firstWord, "display") == 0) { // Check for 'display' command
            if (argCount == 2) { // Verify that the correct number of arguments are provided
                printf("The word is: %s with %d arguments\n", firstWord, argCount);
                display_handling_of_the_commands(args[1]); // Handle the 'display' command
            } else {
                // Print error message if argument count is incorrect
                printf("'display' command requires exactly 2 arguments\n");
            }
        } else {
            // Print error message if the command is not recognized
            printf("Please enter a valid command!\n");
        }
    }

    // Close the socket and exit the program
    close(clt_sdesc); // Close the socket connection
    exit(0); // Exit the program with status 0 (success)
     
}



