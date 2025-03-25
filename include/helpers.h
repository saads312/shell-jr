#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sstream>
#include <cstring>
#include <unistd.h>


// Function to resolve a command's path in the system
std::string resolve_command_path(const std::string& command);

// Function to extract name of executable
std::string getExecutableName(const std::string& path);

// Function to escape characters so that single quotes work inside double quotes
std::string escape_character(std::string arg);


#endif // HELPERS_H