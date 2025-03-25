#include "../include/builtins.h"
#include "../include/parser.h"
#include "../include/helpers.h"
#include "../include/autocomplete.h"
#include "../include/explain.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <unistd.h>

// Function to execute built-in commands
bool handle_builtins(const std::string& input, bool& mustExit) {
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
        if (command == "echo" || command == "type" || command == "pwd" || command == "cd" || command == "exit" || command == "explain") {
            std::cout << "\033[1;36m" << command << "\033[0m is a shell builtin" << std::endl;
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
        std::string new_directory;
        std::string home = getenv("HOME");
        if (input.size() > 2) {
            new_directory = input.substr(3);
            // Remove trailing spaces
            new_directory.erase(new_directory.find_last_not_of(" \n\r\t") + 1);
        } else {
            if (!home.empty()) {
                new_directory = home;
            }
        }
        
        if (new_directory == "~")
            new_directory = home;
        if (!std::filesystem::exists(new_directory)) {
            std::cout << "cd: " << new_directory << ": No such file or directory" << std::endl;
            return true;
        }
        if (new_directory.back() != '/')
            new_directory += '/';
        int change = chdir(new_directory.c_str());
        if (change < 0) return false;
        return true;
    }
    if (input.substr(0, 7) == "explain") {
        std::string commandToExplain = input.substr(8);
        std::string explanation = explainCommand(commandToExplain);
        std::cout << explanation << std::endl;
        return true;
    }
    return false;
}
