#include <iostream>
#include <vector>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <sstream>
#include <functional>
using namespace std;

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

////////////////////////// Implementations //////////////////////////

/*
    Command execution
*/
int launch_cmd(vector<string>& args) {
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

    // Ctr + D
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

int main(int argc, char argv[]) {
    repl_loop();
    return 0;
}