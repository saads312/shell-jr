#include "include/builtins.h"
#include "include/parser.h"
#include "include/autocomplete.h"
#include "include/helpers.h"

#include <iostream>
#include <dirent.h>
#include <sstream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <filesystem>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

// Global variables
std::string destination = ""; // >, >>, 2>, 2>>
bool redirection = false; // >, >>, 2>, 2>>
bool destinationIsError = false; // 2>, 2>>
bool redirAppend = false; // >>
bool destinationErrAppend = false; // 2>>

int main() {
    bool mustExit = false;
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (!mustExit) {
        // reset redirection state for each command
        destination = "";
        redirection = false;

        char hostname[128];
        gethostname(hostname, sizeof(hostname));
        std::string user = getenv("USER") ? getenv("USER") : "user";
        std::string cwd = std::filesystem::current_path();
        std::cout << "\033[2K\r";  // Clear the entire line
        fflush(stdout);        // Flush the output buffer
        rl_on_new_line();      // Inform readline that a new line is being started
        fflush(stdout);        // Flush the output buffer

        // Format prompt with colors
        std::string prompt = "\033[1;32m" + user + "@" + hostname + "\033[0m:\033[1;34m" + getExecutableName(cwd) + " \033[ 0m$ ";

        // Read user input
        rl_attempted_completion_function = my_completion;
        char* c_input = readline(prompt.c_str());

        // Ensure input is valid
        if (!c_input) continue;
        std::string input(c_input);
        free(c_input);

        // Prevent Down Arrow glitch
        if (input.empty()) {
            rl_on_new_line();  // Reset readline state
            rl_replace_line("", 0);  // Clear the current line
            rl_redisplay();  // Refresh the display
            continue;
        }

        
        bool handled = handle_builtins(input, mustExit);
        if (handled) {
            add_history(input.c_str());  // Save only if it's a valid built-in
            write_history("~/.shell_history");
        }

        // if input contains redirection, skip built-in handling
        if (input.find(">") == std::string::npos && handled)
            continue;

        // ignore empty input
        if (input.find_first_not_of(" \t\n") == std::string::npos) {
            continue;   
        }
        
        std::vector<std::string> arguments = parseArguments(input);
        if (arguments.empty()) continue;

        // check for redirection tokens in the parsed arguments
        for (size_t i = 0; i < arguments.size(); i++) {
            if (arguments[i] == ">" || arguments[i] == "1>" || arguments[i] == ">>" || arguments[i] == "1>>") {
                if (arguments[i] == ">>" || arguments[i] == "1>>") {
                    redirAppend = true;
                }
                if (i + 1 >= arguments.size()) {
                    std::cerr << "Syntax error: expected file after " << arguments[i] << std::endl;
                    arguments.clear();
                    break;
                }
                redirection = true;
                destination = arguments[i + 1];
                // remove the redirection operator and its destination
                arguments.erase(arguments.begin() + i, arguments.begin() + i + 2);
                break; // assume only one redirection
            } else if (arguments[i] == "2>" || arguments[i] == "2>>") {
                if (arguments[i] == "2>>") {
                    destinationErrAppend = true;
                }
                if (i + 1 >= arguments.size()) {
                    std::cerr << "Syntax error: expected file after " << arguments[i] << std::endl;
                    arguments.clear();
                    break;
                }
                redirection = true;
                destination = arguments[i + 1];
                destinationIsError = true;
                // remove the redirection operator and its destination
                arguments.erase(arguments.begin() + i, arguments.begin() + i + 2);
                break; // assume only one redirection
            }
        }
        if (arguments.empty()) continue;

        // resolve the command's path using the first token
        std::string filepath = resolve_command_path(arguments[0]);
        if (filepath.empty()) {
            if (arguments[0] != "pwd" && arguments[0] != "cd") {
                std::cout << arguments[0] << ": command not found" << std::endl;
                continue;
            }
        }

        // check that the executable file exists
        std::ifstream file(filepath);
        // use just the name of the command not the path
        std::string final_command = escape_character(getExecutableName(filepath));
        if (file.good()) {
            // build up final command
            for (size_t i = 1; i < arguments.size(); i++) {
                final_command += " " + escape_character(arguments[i]);
            }
            // adding redirections
            if (redirection && !destination.empty()) {
                // if either 2> or 2>>
                if (destinationIsError) {
                    // if 2>>, append error to end
                    if (destinationErrAppend) {
                        final_command += " 2>> " + escape_character(destination);
                        std::system(final_command.c_str());
                        continue;
                    }
                    // if 2>, overwrite file with error
                    final_command += " 2> " + escape_character(destination);
                    std::system(final_command.c_str());
                    continue;
                }
                // if >> or 1>>, append output to end of destination
                if (redirAppend) {
                    final_command += " >> " + escape_character(destination);
                    std::system(final_command.c_str());
                    continue;
                }
                // if > or 1> 
                final_command += " > " + escape_character(destination);
            }

            // uncomment next line for debugging final command
            // std::cout << "Executing: " << final_command << std::endl;

            // execute the final command
            add_history(final_command.c_str());
            write_history(".shell_history");
            std::system(final_command.c_str());
        }
    }
    return 0;
}
