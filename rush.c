// Yaniel Gonzalez Velez

// Project description
/*
This program creates a simple Unix Shell. It continuously asks for input in an 
infinite loop. This program executes commands by creating child processes. It
includes built-in commands, such as exit to exit the program, cd to change
directory, and path to change the shell's environment variable. Finally, it
uses execv() to execute all other commands. In addition, it supports features like
redirection of output, as well as running commands in parallel.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#define true 1

// The one and only error message to print
char error_message[30] = "An error has occurred\n";
// Environment Path variable for the rush shell
char *path[256];

// Function Declarations

// Function that performs the cd built-in command
void cdFun(int count, char *arguments[]);
// Function that performs the path built-in command
void pathFun(char *arguments[]);
// Function that performs execv()
void executeFun(char *path, char *arguments[]);

int main(int argc, char *argv[]){

    // check that there are no command-line arguments
    if(argc > 1){
        write(STDERR_FILENO, error_message, strlen(error_message));
        fflush(stdout);
        exit(1);
    }

    // Setting up variables for the getline() arguments
    char *input = NULL;     // this holds input
    size_t length = 0;      // memory to allocate for input
    size_t char_num;        // number of characters from input

    // set default path values
    path[0] = strdup("/bin");
    path[1] = strdup("/usr/bin");
    path[2] = NULL;

    // Infinite loop
    while(true){
        
        // taking input from rush command prompt
        printf("rush> ");
        fflush(stdout);
        char_num = getline(&input, &length, stdin);

        // handle getline() error
        if(char_num == -1){
            write(STDERR_FILENO, error_message, strlen(error_message));
            fflush(stdout);
            continue;
        }

        // remove the newline character from the input
        if(input[char_num - 1] == '\n'){
            input[char_num - 1] = '\0';
        }

        // parse input by & (for parallel commands)
        char *input_cpy = strdup(input);    // copy of input used for strsep()
        char *argument;                     // holds command argument
        char *commands[256];                // holds the parsed input
        int num = 0;                        // holds the number of items in commands[]

        // while loop to separate the commands by &
        while((argument=strsep(&input_cpy, "&"))!=NULL){
            if(*argument!='\0'){
                commands[num] = argument;
                num++;
            }
        }

        // NULL terminate commands[]
        commands[num] = NULL;

        // create an array to store the pids of the child processes
        pid_t pids[num];

        // loop for the number of different commands provided in input
        int iter = 0;
        for(;commands[iter]!=NULL; iter++){
            // parse input to remove all white space
            char *word;                                 // holds one argument
            char *arguments[256];                       // array of arguments
            int count = 0;                              // tracks number of arguments
            char *input_copy = strdup(commands[iter]);   // copy of input used for strsep

            // while loop that takes away white space and stores the arguments into an array
            while((word=strsep(&input_copy, " \t")) != NULL){
                if(*word != '\0'){
                    arguments[count] = word;
                    count++;
                }
            }

            // NULL terminate the array arguments[]
            arguments[count] = NULL;

            // if not input, then continue to next iter
            if(count == 0){
                continue;
            }


            // exit built-in command
            if(strcmp(arguments[0], "exit") == 0){

                // if there are more arguments than just exit throw error
                if(count != 1){
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    fflush(stdout);
                    continue;
                }
                // else exit using system call exit(0) to end program
                exit(0);

            // cd built-in command call
            }else if(strcmp(arguments[0], "cd") == 0){
                cdFun(count, arguments);
            
            // path built-in command call
            }else if(strcmp(arguments[0], "path") == 0){
                pathFun(arguments);

            }else{

                // create a child process and store its pid on the pid array
                pids[iter] = fork();

                // error handling: if failed to create child process
                if(pids[iter] < 0){
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    fflush(stdout);
                    continue;
                }

                // child process
                if(pids[iter] == 0){
                    
                    // flag that track if access was able to find the command
                    int found = 0;
                    // path index
                    int idxPath = 0;
                    // copy of the path that will have the appeded "/" + command
                    char *ext;

                    // go through all the paths and try to access the command
                    while(path[idxPath]!=NULL){

                        // ext copies at curr index and concatinates "/" and arg[0]
                        ext = strdup(path[idxPath]);
                        strcat(ext, "/");               // concatinates "/"
                        strcat(ext, arguments[0]);      // concatinates command

                        // if accessed, set flag to 1
                        if(access(ext, X_OK) == 0){
                            found = 1;
                            break;
                        }
                        // increment index
                        idxPath++; 

                    }
                    // if access successful, call executeFun
                    if(found){
                        executeFun(ext, arguments);

                    // else, it couldnt access the command, so we throw error
                    }else{
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        fflush(stdout);
                        free(ext);
                        exit(1);
                    }   
                }
            }
        }

        // wait for all the child processes to be done running before giving back rush>
        for(int i=0; i < iter; i++){
            waitpid(pids[i], NULL, 0);
        }
    }
    return 0;
}

// Function Definitions

// Function that performs the cd built-in command
void cdFun(int count, char *arguments[]){

    // more than one argument provided throw error
    if(count == 1 || count > 2){
        write(STDERR_FILENO, error_message, strlen(error_message));
        fflush(stdout);

    // else change directories and throw and error if doesnt exist
    }else{
        if(chdir(arguments[1])!=0){
            write(STDERR_FILENO, error_message, strlen(error_message));
            fflush(stdout);
        }
    }
}


// Function that performs the path built-in command
void pathFun(char * arguments[]){

    // reset path to empty
    for(int i=0; path[i]!=NULL; i++){
        free(path[i]);      // free memory allocated
        path[i] = NULL;     // set the array to NULL
    }

    // add all arguments to the path array except the first one
    // the first argument is the command which shouldn't be on the path
    int j = 0;
    for(int i=1; arguments[i]!=NULL; i++){
        path[j] = strdup(arguments[i]);
        j++;
    }
    // NULL terminate the path array
    path[j] = NULL;
}


// Function that performs execv()
void executeFun(char *path, char *arguments[]){

    // flag variable to hold 1 or more if ">" was found
    int argCheck = 0;
    // track the index of ">" if it exists
    int opIndex;

    // loop to find ">"
    for(int i=0; arguments[i]!=NULL; i++){
        if(strcmp(arguments[i], ">") == 0){
            argCheck++;
            opIndex = i;
        }
    }

    // if we found a ">" we will use redirect
    if(argCheck == 1){

        // Check all edge cases
        // 1: if ">" is the first argument throw error (no command for redirection)
        // 2: if the next argument after the ">" is NULL throw error (no destination to redirect output to)
        // 3: if there are more than 1 argument following the ">" then throw error (can't take multiple destinations to redirect to)
        if(opIndex == 0 || arguments[opIndex+1] == NULL || arguments[opIndex+2]!=NULL){
            write(STDERR_FILENO, error_message, strlen(error_message));
            fflush(stdout);
            exit(1);
        }

        // this array of strings will hold the new arguments to call execv() with
        char *exec_args[256];

        // fill exec_args with arguments up to the index before ">"
        for(int i=0; i < opIndex; i++){
            exec_args[i] = arguments[i];
        }
        // NULL terminate the array
        exec_args[opIndex] = NULL;

        // the file to redirect the output to is the index after we found ">"
        char *outputFile = arguments[opIndex+1];
        
        // close standard output so we can redirect it to a file instead
        close(STDOUT_FILENO);

        // open the file and check that it opens successfully
        if(open(outputFile, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU) == -1){
            write(STDERR_FILENO, error_message, strlen(error_message));
            fflush(stdout);
            exit(1);
        }

        // Finally, we are ready to execute the command now that we have the path and the correct arguments
        execv(path, exec_args);

    // else if the value of argCheck is higher than 1, there were multiple ">" which is not allowed
    }else if(argCheck > 1){
        write(STDERR_FILENO, error_message, strlen(error_message));
        fflush(stdout);
        exit(1);

    // else there were no ">" found so we execute as normal (no redirection)
    }else{
        execv(path, arguments);
    }
}