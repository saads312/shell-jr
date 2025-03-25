#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <sstream>
#include <cstring>

// Function to parse command and arguments from overall user input string
std::vector<std::string> parseArguments(std::string input);

#endif // PARSER_H