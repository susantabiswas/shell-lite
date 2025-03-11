/**
 * @file shell.cpp
 * @brief A lightweight shell implementation that supports both built-in and external commands
 * 
 * This shell provides a command-line interface with the following features:
 * - Basic REPL (Read-Evaluate-Print Loop) interface
 * - Built-in commands: cd, help, exit
 * - External command execution using fork/exec pattern
 * - Command line parsing with argument tokenization
 * - Child process management and wait status handling
 */
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <functional>
#include <thread>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

////////////////////////// Prototypes //////////////////////////
// External commands
int execute_cmd(char** args, size_t n_args);
int launch_cmd(char** args);

// Built-ins
int cmd_cd(char** args);
int cmd_help(char** args);
int cmd_exit(char** args);

// shell operations
void print_prompt();
pair<char**, size_t> tokenize_line(char* args);
char* read_line();
void repl_loop();

/*
    Constants
*/
const string PROMPT = "> ";

// built-in function template
using func = function<int(char**)>;

// mapping of built-in commands to their respective functions
unordered_map<string, func> built_in_cmds = {
    {"cd", cmd_cd},
    {"help", cmd_help},
    {"exit", cmd_exit}
};

unordered_map<string, string> built_in_description = {
    {"cd", "Change the current working directory"},
    {"help", "Help menu for the shell"},
    {"exit", "Exit the shell"}
};

////////////////////////// Implementations //////////////////////////

/*
    Command execution
*/

/**
 * @brief Launches an external command in a child process
 * @param args NULL-terminated array of command arguments
 * @return 1 on success, 0 on failure
 */
int launch_cmd(char** args) {
    // launch the command in a child process
    pid_t pid = fork();
    
    // child process
    if (pid == 0) {
        if(execvp(args[0], args) == -1) {
            perror("[shell] Error launching command.");
            return 0;
        }
    }
    // error forking
    else if(pid < 0) {
        cerr << "Error forking process: " <<  getpid() << endl;
        perror("[shell] Error forking child process.");
        return 0;
    }
    // parent process
    else {
        int status;
        do {
            // wait till the child is not stopped, when
            // it does, return the status of the child
            waitpid(pid, &status, WUNTRACED);
        }
        // the child process state can change multiple times
        // during its execution and due to that it can be at "Stopped"
        // state.
        // WIFEXITED(status) returns true if the child terminated normally
        // WIFSIGNALED(status) returns true if the child process was terminated by a signal
        // So we continue only if the child process didnt exit or wasnt
        // signalled to stop 
        while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    
    return 1;
}

/**
 * @brief Executes a command based on whether built-in or external command.
 * @param args Array of command arguments (NULL-terminated)
 * @param n_args Number of arguments
 * @return 1 on success, 0 on failure
 */
int execute_cmd(char** args, size_t n_args) {
    if (n_args == 0) {
        cout << "Empty command entered, please enter your input..." << endl;
        return 1;
    }

    // check if it is one of the built-in commands
    if(built_in_cmds.count(string(args[0]))) {
        return built_in_cmds[args[0]](args);
    }

    // Launch the external command
    return launch_cmd(args);
}

/*
    Built-in commands
    @brief These are native commands of the shell program. These
    are mostly used for shell related operations.
*/
/*
 * @brief Change the current working directory
 * @param args: The path to change the directory to
 * @remark The reason 'cd' is a built-in instead of using an
 * external command is because the commands are launched as child process,
 * so an external cd command will only change the dir for the child process
 * instead of the parent process.
 */
int cmd_cd(char** args) {
    // nullptr is the last element of args, if there
    // is no path provided, return an error
    if (args[1] == nullptr) {
        cerr << "No path provided. Usage: cd <path>" << endl;
        return 0;
    }

    if (chdir(args[1]) != 0) {
        perror("[shell] Error changing directory.");
        return 0;
    }

    return 1;
}

/**
 * @brief Built-in command to display help information
 * @param args Command arguments (unused)
 * @return Always returns 1
 */
int cmd_help(char** args) {
    cout << "Shell help" << endl;
    cout << "Following built-in commands are supported" << endl;

    for(auto cmd: built_in_cmds) {
        cout << cmd.first << ": " << built_in_description[cmd.first] << endl;
    }

    return 1;
}

/**
 * @brief Built-in command to exit the shell
 * @param args Command arguments (unused)
 * @return Never returns; calls exit()
 */
int cmd_exit(char** args) {
    cout << "Exiting shell" << endl;
    exit(EXIT_SUCCESS);
    return 1;
}

/*
    Shell operations
*/

void print_prompt() {
    cout << PROMPT;
}

/**
 * @brief Reads a line of input from standard input
 * @return Dynamically allocated string containing user input
 */
char* read_line() {
    char* line = nullptr;
    // when buff_size is 0, getline will allocate the buffer memory
    // suitable for holding the input.
    size_t buff_size = 0;

    int chars_read = getline(&line, &buff_size, stdin);

    if (chars_read == -1) {
        if(feof(stdin)) {
            cerr << "EOF reached, exiting" << endl;
            exit(EXIT_SUCCESS);
        }

        perror("[shell] Error reading input.");
        exit(EXIT_FAILURE);
    }
    return line;
}

/**
 * @brief Parses a command line into tokens
 * @param line Input string to tokenize
 * @return Pair of {token array, token count}
 */
pair<char**, size_t> tokenize_line(char* line) {
    if (!line)
        return { nullptr, 0 };
        
    int tokens_list_len = 32;
    const char* DELIM = " \t\r\n\a";

    char* token = nullptr;
    char** tokens = (char**) malloc(sizeof(char*) * tokens_list_len);
    
    if (!tokens) {
        cerr << "Error allocating memory for tokens" << endl;
        exit(EXIT_FAILURE);
    }
    
    token = strtok(line, DELIM);
    int pos = 0;

    while(token) {
        tokens[pos++] = token;
        token = strtok(NULL, DELIM);

        // check if the tokens list is full
        // if reserve the last element for the NULL terminator,
        // execvp needs the last element to be NULL
        if (pos >= tokens_list_len - 1) {
            tokens_list_len *= 2;
            
            char** new_tokens = (char**)realloc(tokens, tokens_list_len * sizeof(char*));

            if (!new_tokens) {
                perror("[shell] Error allocating memory for tokens.");
                // Since the resizing attempt failed, free the memory of existing
                // tokens list
                free(tokens);
                exit(EXIT_FAILURE);
            }

            // realloc attempt was successful
            tokens = new_tokens;
        }
    }

    // excevp requires the last element to be NULL.
    tokens[pos] = nullptr;
    
    return { tokens, pos };
}

/**
 * @brief Main shell loop that reads and executes commands
 * 
 * This function implements the shell's read-evaluate-print loop:
 * 1. Display prompt
 * 2. Read user input
 * 3. Parse input into tokens
 * 4. Execute command
 * 5. Repeat
 */
void repl_loop() {
    char* line;
    char** args;

    cout << "Starting shell...." << endl;
    while(true) {
        print_prompt();
        line = read_line();

        if (sizeof(line) == 0) {
            cout << "Empty input received, please enter a command" << endl;
            continue;
        }

        // tokenize the command
        auto [args, n_args] = tokenize_line(line);
        execute_cmd(args, n_args);

        // strtok when used for tokenization returns the ptr to 
        // positions in the original string line. So, args is just
        // a block of memory where each block is just pointing to
        // addresses in the original line.
        // Once free(line) happens, the string related memory is freed, 
        // so args can also be freed using free(args) instead of freeing
        // individual elements of args.
        free(line);
        free(args);
    }
}

int main(int argc, char** argv) {
    repl_loop();
    return 0;
}