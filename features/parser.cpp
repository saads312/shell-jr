#include "../include/parser.h"
#include "../include/builtins.h"
#include "../include/helpers.h"
#include "../include/autocomplete.h"

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
