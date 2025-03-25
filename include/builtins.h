#include <iostream>
#include <string>

#ifndef BUILTINS_H
#define BUILTINS_H

// Function to check if a command is a builtin command
bool handle_builtins(const std::string& input, bool& mustExit);

#endif // BUILTINS_H