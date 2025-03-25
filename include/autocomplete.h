#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <string>
#include <vector>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include <sstream>
#include <cstring>
#include <unistd.h>

// Generator function: called repeatedly with state values.
char *builtin_generator(const char *text, int state);
char *execs_generator(const char *text, int state);

// Function to complete the command line
char **my_completion(const char *text, int start, int end);

#endif // AUTOCOMPLETE_H
