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

// Function to extract name of executable
std::string getExecutableName(const std::string& path) {
    size_t pos = path.find_last_of('/');
    return (pos == std::string::npos) ? path : path.substr(pos + 1);
}

// Function to parse command and arguments from overall user input string
std::vector<std::string> parseArguments(std::string input) {
    std::stringstream full_command(input);
    std::vector<std::string> arguments;
    std::string argument;
    char active_quote = '\0';
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        // if the character is a backslash
        if (c == '\\') {
            if (i + 1 < input.length()) {
                char next_char = input[i+1];
                // if inside a double quote
                if (active_quote == '"') {
                    // if inside a double quote AND is one of the special characters
                    if (next_char == '\\' || next_char == '$' || next_char == '"' || next_char == '\n') {
                        argument += next_char;
                        i++;
                        continue;
                    } else {
                        argument += c;  // treat backslash literally in double quotes if not special
                        continue;
                    }
                // if no quotes, backslash escapes next character
                } else if (active_quote == '\0') {
                    argument += next_char;
                    i++;
                    continue;
                } 
                // inside single quotes backslash is literal
                else {
                    argument += c;
                    continue;
                }
            } else {
                argument += c;
                continue;
            }
        } 
        // if character is a single quote or double quote
        else if (c == '\'' || c == '"') {
            if (active_quote == '\0') 
                active_quote = c;
            else if (active_quote == c)
                active_quote = '\0';
            else 
                argument += c;
        } 
        // if character is a space
        else if (c == ' ' && active_quote == '\0') {
            if (!argument.empty()) {
                arguments.push_back(argument);
                argument.clear();
            }
        }
        else {
            argument += c;
        }
    }
    if (!argument.empty()) 
        arguments.push_back(argument);
    return arguments;
}

// Function to escape characters so that single quotes work inside double quotes
std::string escape_character(std::string arg) {
    std::string result = "'";
    for (size_t i = 0; i < arg.length(); i++) {
        char c = arg[i];
        if (c == '\'') {
            result += "'\\''";
        } else {
            result += c;
        }
    }
    result += "'";
    return result;
}

// Function to resolve a command's path in the system
std::string resolve_command_path(const std::string& command) {
    if (command == "pwd") return "";
    char* path_env = getenv("PATH");
    if (!path_env) return "";

    std::stringstream ss(path_env);
    std::string directory;
    // get all directories in path variable separated by :
    while (std::getline(ss, directory, ':')) {
        DIR* dir = opendir(directory.c_str());
        if (!dir) continue;
        struct dirent* entry;
        // read directory and compare name with given command name
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, command.c_str()) == 0) {
                closedir(dir);
                return directory + "/" + command;
            }
        }
        closedir(dir);
    }
    return "";
}

// Function to execute built-in commands
bool handle_builtin_commands(const std::string& input, bool& mustExit) {
    // pnly process built-in commands if there is no redirection operator
    if (input.find('>') != std::string::npos) return false;
    
    if (input == "exit" || input == "exit 0") {
        mustExit = true;
        return true;
    }
    // handle echo command
    if (input.substr(0, 4) == "echo") {
        std::vector<std::string> arguments = parseArguments(input);
        std::string final_output;
        while (arguments.size() > 1) {
            std::string incremental_output = arguments.back();
            arguments.pop_back();
            final_output = incremental_output + " " + final_output;
        }
        std::cout << final_output << std::endl;
        return true;
    }
    // handle type command
    if (input.substr(0, 4) == "type") {
        std::string command = input.substr(5);
        if (command == "echo" || command == "type" || command == "pwd" || command == "cd") {
            std::cout << command << " is a shell builtin" << std::endl;
            return true;
        }
        if (command == "exit" || command == "exit 0") {
            std::cout << command << " is a shell builtin" << std::endl;
            return true;
        }
        std::string path = resolve_command_path(command);
        if (!path.empty())
            std::cout << command << " is " << path << std::endl;
        else
            std::cout << command << ": not found" << std::endl;
        return true;
    }
    // handle pwd
    if (input.substr(0, 3) == "pwd") {
        std::string cwd = std::filesystem::current_path();
        std::cout << cwd << std::endl;
        return true;
    }
    // handle cd
    if (input.substr(0, 2) == "cd") {
        std::string new_directory = input.substr(3);
        std::string home = getenv("HOME");
        if (new_directory == "~")
            new_directory = home;
        if (!std::filesystem::exists(new_directory))
            std::cout << "cd: " << new_directory << ": No such file or directory" << std::endl;
        int change = chdir(new_directory.c_str());
        if (change < 0) return false;
        return true;
    }
    return false;
}

// Generator function: called repeatedly with state values.
char *builtin_generator(const char *text, int state) {
    static int list_index, len;
    const char *builtins[] = { "echo", "exit", nullptr };

    if (state == 0) {
        list_index = 0;
        len = strlen(text);
    }

    while (builtins[list_index]) {
        const char *name = builtins[list_index];
        list_index++;
        if (strncmp(name, text, len) == 0) {
            return strdup(name);  // Return a malloc'd string.
        }
    }
    return nullptr;
}

char *execs_generator(const char *text, int state) {
    static std::vector<std::string> execs;
    static size_t index;
    static size_t len;

    if (state == 0) {
        execs.clear();
        index = 0;
        len = strlen(text);

        char* path_env = getenv("PATH");
        if (!path_env) return nullptr;

        std::stringstream ss(path_env);
        std::string directory;
        // get all directories in path variable separated by :
        while (std::getline(ss, directory, ':')) {
            DIR* dir = opendir(directory.c_str());
            if (!dir) continue;
            struct dirent* entry;
            // read directory and compare name with typed prefix
            while ((entry = readdir(dir)) != nullptr) {
                if (strncmp(entry->d_name, text, len) == 0) {
                    execs.push_back(entry->d_name);
                }
            }
            closedir(dir);
        }
    }

    if (index < execs.size()) {
        return strdup(execs[index++].c_str());
    }

    return nullptr;
}

// Completion function: called by readline when the user hits TAB.
char **my_completion(const char *text, int start, int end) {
    (void)end; // Unused parameter.
    char **matches = nullptr;
    // If we're completing the command name (first token), use our generator.
    if (start == 0) {
        matches = rl_completion_matches(text, builtin_generator);
        if (!matches) {
            matches = rl_completion_matches(text, execs_generator);
        }
    }
    return matches;
}


int main() {
    bool mustExit = false;
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (!mustExit) {
        // reset redirection state for each command
        destination = "";
        redirection = false;

        // std::cout << "$ ";
        rl_attempted_completion_function = my_completion;
        char* c_input = readline("$ ");
        if (!c_input) continue;
        std::string input(c_input);
        free(c_input);
        
        // if input contains redirection, skip built-in handling
        if ((!input.find(">")) != std::string::npos && handle_builtin_commands(input, mustExit))
            continue;
        
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
            std::system(final_command.c_str());
        }
    }
    return 0;
}
