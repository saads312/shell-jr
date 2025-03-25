# Shell Jr

### Welcome to Shell Jr!
A custom bash-style shell written in C++ with the intention of making the Command Line Interface less intimidating for new users.

<p align="center">
  <img src="https://github.com/user-attachments/assets/efff0cd3-f5c1-4007-8e29-edd53e51415e" alt="scared bowser jr" style="border-radius: 80%">
</p>


## Overview

Shell Jr is designed to offer a user-friendly and educational command-line experience. It comes with a clean, colored prompt, built-in commands, and even an AI-powered `explain` command to help new users understand what each command does. Whether you're a beginner or looking to polish your CLI skills, Shell Jr offers a gentle introduction to the command line.

## Features

- **Beginner-Friendly Interface**  
  - Custom, colored prompt displaying username, hostname, and current directory  
  - Ignores empty or whitespace-only commands  
  - Built-in tutorial (coming soon) to guide new users

- **Regular Command-Line Functionalities**  
  - Standard builtin commands like `cd`, `pwd`, `echo`, `exit`, `type`, and more  
  - Special built-ins that provide extra help or insights
  - Autocompletion and command history
  - Redirection support for standard outptut and errors using `>`, `2>`, etc

- **AI-powered `explain` Command**  
  - Type a command with `explain "command"` to receive a plain-English explanation  
  - Uses GPT-4o via GitHub’s AI inference endpoint for on-demand assistance
 
## Usage Example
```bash
saad@Syeds-MacBook-Air.local:shell-jr $ explain cd test2 && mkdir test3
The "cd" keyword changes the current directory to "test2", which is the argument provided by the
user. The "mkdir" keyword creates a new directory named "test3" if the first command succeeds.
saad@Syeds-MacBook-Air.local:shell-jr $ explain rm -rf example_directory
The "rm" command is used to remove files or directories. The "-rf" keyword forces deletion (-f) and
allows recursive removal of directories (-r), while "example_directory" is the name of the directory the user wants to delete.
```

## Tech Stack
- **Language**: C++23
- **Libraries**:
  - GNU Readline
  - libcurl
  - nlohmann/json
- **AI Integration**:
  - Azure OpenAI Service GPT-4o API
 
