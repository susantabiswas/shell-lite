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
int execute_cmd(vector<string>& args);
int launch_cmd(vector<string>& args);

// Built-ins
int cmd_cd(vector<string>& args);
int cmd_help(vector<string>& args);
int cmd_exit(vector<string>& args);

// shell operations
void print_prompt();
vector<string> parse_args(string& args);
string read_line();
void repl_loop();

/*
    Constants
*/
const string PROMPT = "> ";
const int BUFFER_LEN = 1024;

using func = function<int(vector<string>&)>;

// mapping of built-in commands to their respective functions
unordered_map<string, func> built_in_cmds = {
    {"cd", cmd_cd},
    {"help", cmd_help},
    {"exit", cmd_exit}
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

int launch_cmd(vector<string>& args) {
    // launch the command in a child process
    pid_t pid = fork();
    char** c_args = (char**)malloc(sizeof(char*) * args.size() + 1);

    int i = 0;
    for(auto arg: args) {
        c_args[i++] = strdup(arg.c_str());
    }

    // child process
    if (pid == 0) {
        if(execvp(args[0].c_str(), c_args) == -1) {
            free_args(c_args);
            perror("[shell]");
            return 0;
        }
    }
    // error forking
    else if(pid < 0) {
        cerr << "Error forking process: " <<  getpid() << endl;
        free_args(c_args);
        perror("[shell]");
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
    
    free_args(c_args);
    return 1;
}

int execute_cmd(vector<string>& args) {
    if (args.empty()) {
        cout << "Empty command entered, please enter your input..." << endl;
        return 1;
    }

    // check if it is one of the built-in commands
    if(built_in_cmds.count(args[0])) {
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

string read_line() {
    string line;

    getline(cin, line);

    if (cin.fail()) {
        cerr << "Error reading input" << endl;
        exit(EXIT_FAILURE);
    }

    // Ctr + D at the end of some input
    if (cin.eof()) {
        cout << "EOF reached" << endl;
        exit(EXIT_SUCCESS);
    }

    return line;
}

vector<string> parse_args(string& line) {
    vector<string> args;
    string token;

    istringstream ss(line);
    char delimiter = ' ';
    
    while(getline(ss, token, delimiter)) {
        args.push_back(token);
    }
    return args;
}

void repl_loop() {
    string line;
    vector<string> args;

    while(true) {
        print_prompt();
        line = read_line();
        // tokenize the command
        args = parse_args(line);
        execute_cmd(args);
    }
}

int main(int argc, char** argv) {
    repl_loop();
    return 0;
}