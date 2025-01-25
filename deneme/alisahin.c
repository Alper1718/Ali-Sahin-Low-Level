#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MEMORY_SIZE 30000
#define MAX_LINE_LENGTH 256
#define MAX_FUNCTIONS 100
#define MAX_COMMANDS_PER_FUNCTION 1000

typedef struct {
    char *name;
    char *commands[MAX_COMMANDS_PER_FUNCTION];
    int command_count;
    bool is_sudo;
} Function;

char *memory;
size_t pointer = 0;
Function functions[MAX_FUNCTIONS];
int function_count = 0;
bool sudo_mode = false;

void error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

void execute_line(const char *line);
void execute_function(const char *function_name);

void init_memory() {
    memory = (char *)calloc(MEMORY_SIZE, sizeof(char));
    if (!memory) {
        error("Failed to allocate memory.");
    }
}

void free_memory() {
    free(memory);
}

void add_function(const char *name, bool is_sudo) {
    if (function_count >= MAX_FUNCTIONS) {
        error("Too many functions defined.");
    }

    // Check for existing function and override if in sudo mode
    for (int i = 0; i < function_count; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            if (is_sudo || !functions[i].is_sudo) {
                free(functions[i].name);
                for (int j = 0; j < functions[i].command_count; j++) {
                    free(functions[i].commands[j]);
                }
                functions[i].name = strdup(name);
                functions[i].command_count = 0;
                functions[i].is_sudo = is_sudo;
                return;
            } else {
                error("Cannot override a sudo function without sudo mode.");
            }
        }
    }

    functions[function_count].name = strdup(name);
    functions[function_count].command_count = 0;
    functions[function_count].is_sudo = is_sudo;
    function_count++;
}

Function *get_function(const char *name) {
    for (int i = 0; i < function_count; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            return &functions[i];
        }
    }
    return NULL;
}

void add_command_to_function(Function *function, const char *command) {
    if (function->command_count >= MAX_COMMANDS_PER_FUNCTION) {
        error("Too many commands in function.");
    }
    function->commands[function->command_count++] = strdup(command);
}

void parse_file(const char *filename);

void execute_line(const char *line) {
    char *tokens[MAX_COMMANDS_PER_FUNCTION];
    char *line_copy = strdup(line);
    int token_count = 0;
    
    char *token = strtok(line_copy, " ");
    while (token) {
        tokens[token_count++] = token;
        token = strtok(NULL, " ");
    }

    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "ali") == 0) {
            pointer++;
            if (pointer >= MEMORY_SIZE) {
                error("Memory pointer out of bounds (right).");
            }
        } else if (strcmp(tokens[i], "sahin") == 0) {
            if (pointer == 0) {
                error("Memory pointer out of bounds (left).");
            }
            pointer--;
        } else if (strcmp(tokens[i], "kas") == 0) {
            memory[pointer] = (memory[pointer] + 1) % 256;
        } else if (strcmp(tokens[i], "tek") == 0) {
            memory[pointer] = (memory[pointer] - 1) % 256;
        } else if (strcmp(tokens[i], "kasistan") == 0) {
            putchar(memory[pointer]);
        } else if (strcmp(tokens[i], "alisah") == 0) {
            int user_input;
            scanf("%d", &user_input);
            memory[pointer] = (memory[pointer] + user_input) % 256;
        } else if (strcmp(tokens[i], "tekkas") == 0) {
            if (memory[pointer] == 0) {
                return;
            }
        } else if (strcmp(tokens[i], "alisahin") == 0) {
            if (memory[pointer] != 0) {
                return;
            }
        } else if (get_function(tokens[i])) {
            execute_function(tokens[i]);
        } else {
            error("Unknown command.");
        }
    }

    free(line_copy);
}

void execute_function(const char *function_name) {
    Function *function = get_function(function_name);
    if (!function) {
        error("Function not defined.");
    }

    for (int i = 0; i < function->command_count; i++) {
        execute_line(function->commands[i]);
    }
}

void parse_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        // Try adding .alisahin extension
        char modified_filename[MAX_LINE_LENGTH];
        snprintf(modified_filename, sizeof(modified_filename), "%s.alisahin", filename);
        file = fopen(modified_filename, "r");
        if (!file) {
            error("Could not open file.");
        }
    }

    char line[MAX_LINE_LENGTH];
    Function *current_function = NULL;

    while (fgets(line, sizeof(line), file)) {
        char *trimmed_line = strtok(line, "\n");
        if (!trimmed_line || trimmed_line[0] == ';') {
            continue;
        }

        if (strstr(trimmed_line, "sudo")) {
            sudo_mode = true;
        } else if (strstr(trimmed_line, "nejatjobs")) {
            char *function_name = strtok(trimmed_line + 10, " ");
            if (!function_name) {
                error("Invalid function definition.");
            }
            add_function(function_name, sudo_mode);
            current_function = &functions[function_count - 1];
        } else if (strcmp(trimmed_line, "}") == 0) {
            current_function = NULL;
            sudo_mode = false; // Reset sudo mode after function definition ends
        } else if (current_function) {
            add_command_to_function(current_function, trimmed_line);
        } else if (strstr(trimmed_line, "unibrow")) {
            char *import_filename = strtok(trimmed_line + 8, " ");
            if (!import_filename) {
                error("Invalid import statement.");
            }
            parse_file(import_filename);
        } else {
            execute_line(trimmed_line);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    init_memory();

    parse_file(argv[1]);

    free_memory();

    return EXIT_SUCCESS;
}
