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
void free_args(char**& args) {
    for(int i = 0; i < sizeof(args) / sizeof(char*); i++)
        free(args[i]);
    free(args);
}

int launch_cmd(char** args) {
    // launch the command in a child process
    pid_t pid = fork();
    
    // child process
    if (pid == 0) {
        if(execvp(args[0], args) == -1) {
            free_args(args);
            perror("[shell] Error launching command.");
            return 0;
        }
    }
    // error forking
    else if(pid < 0) {
        cerr << "Error forking process: " <<  getpid() << endl;
        free_args(args);
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
    
    free_args(args);
    return 1;
}

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
*/
int cmd_cd(vector<string>& args) {
    return 1;
}

int cmd_help(vector<string>& args) {
    return 1;
}

int cmd_exit(vector<string>& args) {
    return 1;
}

/*
    Shell operations
*/

void print_prompt() {
    cout << PROMPT;
}

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

void repl_loop() {
    char* line;
    char** args;

    while(true) {
        print_prompt();
        line = read_line();

        if (sizeof(line) == 0) {
            cout << "Empty input received, please enter a command" << endl;
            continue;
        }

        cout << "Input: "<< line<<endl;
        // tokenize the command
        auto [args, n_args] = tokenize_line(line);
        execute_cmd(args, n_args);
    }
}

int main(int argc, char** argv) {
    repl_loop();
    return 0;
}