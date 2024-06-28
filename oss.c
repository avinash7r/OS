#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_INPUT_SIZE 1024

void display_prompt() {
    printf("myshell> ");
}

char* read_input() {
    char *input = (char *)malloc(MAX_INPUT_SIZE);
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
        free(input);
        return NULL;
    }
    return input;
}

char** parse_input(char *input) {
    char **tokens = malloc(MAX_INPUT_SIZE * sizeof(char*));
    char *token;
    int position = 0;

    token = strtok(input, " \t\r\n");
    while (token != NULL) {
        tokens[position] = token;
        position++;
        token = strtok(NULL, " \t\r\n");
    }
    tokens[position] = NULL;
    return tokens;
}

void execute_command(char **args) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char command[MAX_INPUT_SIZE] = "";

    for (int i = 0; args[i] != NULL; i++) {
        strcat(command, args[i]);
        if (args[i + 1] != NULL) {
            strcat(command, " ");
        }
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(
        NULL,           // No module name (use command line)
        command,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi             // Pointer to PROCESS_INFORMATION structure
    )) {
        fprintf(stderr, "myshell: CreateProcess failed (%d).\n", GetLastError());
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int main() {
    char *input;
    char **args;

    while (1) {
        display_prompt();
        input = read_input();
        if (input == NULL) {
            break; // End of input
        }

        args = parse_input(input);
        if (args[0] != NULL) {
            execute_command(args);
        }

        free(input);
        free(args);
    }

    return 0;
}
