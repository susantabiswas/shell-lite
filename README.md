# Shell Lite

Lightweight shell implementation to understand the underlying mechanics of Unix/Linux shells.

## Overview

Shell Lite is a minimal shell implementation written in C++ that demonstrates core shell functionalities. This project was created to understand the fundamental operations of command-line interfaces.

The shell supports both built-in commands (cd, help, exit) and external commands through the standard PATH lookup.

## Output
```
              ════════════════════════════════════
                      Shell lite started....
               ════════════════════════════════════

    ███████╗██╗  ██╗███████╗██╗     ██╗         ██╗     ██╗████████╗███████╗
    ██╔════╝██║  ██║██╔════╝██║     ██║         ██║     ██║╚══██╔══╝██╔════╝
    ███████╗███████║█████╗  ██║     ██║         ██║     ██║   ██║   █████╗
    ╚════██║██╔══██║██╔══╝  ██║     ██║         ██║     ██║   ██║   ██╔══╝
    ███████║██║  ██║███████╗███████╗███████╗    ███████╗██║   ██║   ███████╗
    ╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝    ╚══════╝╚═╝   ╚═╝   ╚══════╝

    Type 'help' for available commands

> pwd
/mnt/c/Users/dev/Desktop/Dev/github/shell-lite

> ls -la
total 168
drwxrwxrwx 1 seeker seeker   4096 Mar 11 21:11 .
drwxrwxrwx 1 seeker seeker   4096 Mar  8 17:42 ..
drwxrwxrwx 1 seeker seeker   4096 Mar 11 21:09 .git
drwxrwxrwx 1 seeker seeker   4096 Mar 10 22:53 .vscode
-rwxrwxrwx 1 seeker seeker   1092 Mar  8 17:55 LICENSE
-rwxrwxrwx 1 seeker seeker   3305 Mar 11 21:08 README.md
-rwxrwxrwx 1 seeker seeker    517 Mar 11 20:20 makefile
-rwxrwxrwx 1 seeker seeker 149200 Mar 11 21:11 shell
-rwxrwxrwx 1 seeker seeker   8871 Mar 11 20:21 shell.cpp

> cd ..

> pwd
/mnt/c/Users/dev/Desktop/Dev/github
>
```

## Project Setup

>Note: This shell uses `fork()` system call and hence will not work on a Windows machine. You can use WSL in Windows to run this.
### Prerequisites
- `C++ compiler` (GCC or Clang recommended)
- `Make` (optional, for build automation)
- Unix-like environment (Linux, macOS, WSL on Windows)

### Building the Project

Clone the repository and build the shell:

```bash
git clone https://github.com/yourusername/shell-lite.git
cd shell-lite
g++ -o shell shell.cpp -std=c++17
```

#### Using Make (recommended)

A Makefile is included for easier building:

```bash
# Build the shell
make

# Clean build artifacts
make clean

# Rebuild the project
make rebuild
```

The Makefile handles compilation flags and dependencies automatically, making it the recommended build method.

## Basic Usage

### Running the Shell

To start the shell:

```bash
./shell
```

You'll be presented with a simple prompt: `> `

### Built-in Commands

Shell Lite supports the following built-in commands:

| Command | Description | Usage |
|---------|-------------|-------|
| `cd` | Change the current directory | `cd <directory>` |
| `help` | Display available commands | `help` |
| `exit` | Exit the shell | `exit` |

### External Commands

Any command not recognized as a built-in will be treated as an external command and executed using the PATH lookup mechanism. For example:

```
> ls -l
> echo "Hello World"
> cat README.md
```

## Implementation Details

The shell follows a simple REPL (Read-Evaluate-Print Loop) pattern:

1. **Read**: Reads user input from the command line
2. **Tokenize**: Splits the input into command and arguments
3. **Execute**: 
   - Checks if the command is a built-in
   - If not, forks a new process and executes the external command by replacing the process space of child process with external program.
4. **Wait**: Waits for the command to complete
5. **Loop**: Returns to step 1

## Acknowledgements
- https://brennan.io/2015/01/16/write-a-shell-in-c/
