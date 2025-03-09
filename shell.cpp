#include <iostream>
#include <vector>
using namespace std;

int execute_cmd(vector<string>& args);
int launch_launch(vector<string>& args);

int cmd_cd(vector<string>& args);
int cmd_help(vector<string>& args);
int cmd_exit(vector<string>& args);

vector<string> parse_args(string args);
vector<string> read_input();
int repl_loop();

int main(int argc, char argv[]) {

    return 0;
}