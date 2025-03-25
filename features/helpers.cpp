#include "../include/helpers.h"
#include "../include/builtins.h"
#include "../include/parser.h"
#include "../include/autocomplete.h"

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

// Function to extract name of executable
std::string getExecutableName(const std::string& path) {
    size_t pos = path.find_last_of('/');
    return (pos == std::string::npos) ? path : path.substr(pos + 1);
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