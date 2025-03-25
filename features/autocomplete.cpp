#include "../include/autocomplete.h"
#include "../include/builtins.h"
#include "../include/parser.h"
#include "../include/helpers.h"


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
