/*
 * FFI Example - WinuxCmd C API Demo
 * Demonstrates how to use WinuxCmd FFI library for high-performance command execution
 *
 * This example:
 * 1. Starts the daemon once
 * 2. Executes multiple commands via JSON-RPC style requests
 * 3. Measures and reports performance metrics
 *
 * Compile: gcc -o ffi_example ffi_example.c -I../../src/ffi -L../../build-release -lwinuxcmd
 * Run: ./ffi_example
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

// FFI API (from ffi.h)
#ifdef _WIN32
    #define WINUX_API __declspec(dllimport)
#else
    #define WINUX_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char* stdout_text;
    char* stderr_text;
    size_t stdout_size;
    size_t stderr_size;
    int exit_code;
} WinuxCmdResponse;

typedef struct {
    char* command;
    char** args;
    int arg_count;
    char* cwd;
    char* stdout_text;
    char* stderr_text;
    size_t stdout_size;
    size_t stderr_size;
    int exit_code;
    char* error_code;
    char* error_message;
} WinuxCmdRequest;

WINUX_API int winux_execute(const char* command, const char** args, int arg_count,
                              const char* cwd, char** output, char** error,
                              size_t* output_size, size_t* error_size);
WINUX_API void winux_free_buffer(char* buffer);
WINUX_API void winux_free_commands_array(char** commands, int count);
WINUX_API int winux_is_daemon_available(void);
WINUX_API const char* winux_get_version(void);
WINUX_API int winux_get_all_commands(char*** commands, int* count);

#ifdef __cplusplus
}
#endif

// Performance statistics
typedef struct {
    long long total_requests;
    long long successful_requests;
    long long failed_requests;
    double total_time_ms;
    double avg_time_ms;
    double min_time_ms;
    double max_time_ms;
} PerformanceStats;

void init_stats(PerformanceStats* stats) {
    stats->total_requests = 0;
    stats->successful_requests = 0;
    stats->failed_requests = 0;
    stats->total_time_ms = 0;
    stats->avg_time_ms = 0;
    stats->min_time_ms = 999999.0;
    stats->max_time_ms = 0.0;
}

void update_stats(PerformanceStats* stats, double elapsed_ms, int success) {
    stats->total_requests++;
    stats->total_time_ms += elapsed_ms;

    if (success) {
        stats->successful_requests++;
        if (elapsed_ms < stats->min_time_ms) {
            stats->min_time_ms = elapsed_ms;
        }
        if (elapsed_ms > stats->max_time_ms) {
            stats->max_time_ms = elapsed_ms;
        }
    } else {
        stats->failed_requests++;
    }

    stats->avg_time_ms = stats->total_time_ms / stats->total_requests;
}

void print_stats(const PerformanceStats* stats) {
    printf("\n========== Performance Statistics ==========\n");
    printf("Total Requests:        %lld\n", stats->total_requests);
    printf("Successful:            %lld\n", stats->successful_requests);
    printf("Failed:                %lld\n", stats->failed_requests);
    printf("Success Rate:          %.2f%%\n",
           (stats->successful_requests * 100.0) / stats->total_requests);
    printf("Total Time:            %.2f ms\n", stats->total_time_ms);
    printf("Average Time:           %.2f ms\n", stats->avg_time_ms);
    printf("Min Time:              %.2f ms\n", stats->min_time_ms);
    printf("Max Time:              %.2f ms\n", stats->max_time_ms);
    printf("============================================\n\n");
}

double get_time_ms() {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1000.0 / frequency.QuadPart;
}

int main() {
    printf("WinuxCmd FFI Example - Performance Test\n");
    printf("====================================\n\n");

    // Get version
    const char* version = winux_get_version();
    printf("WinuxCmd Version: %s\n\n", version ? version : "unknown");

    // Check if daemon is available
    int daemon_available = winux_is_daemon_available();
    printf("Daemon Available: %s\n\n", daemon_available ? "Yes" : "No");

    // Check daemon availability again
    daemon_available = winux_is_daemon_available();
    printf("Daemon Available after init: %s\n\n", daemon_available ? "Yes" : "No");

    // Test: Get all available commands
    printf("Test: Getting all available commands...\n");
    char** commands = NULL;
    int command_count = 0;
    int get_cmds_result = winux_get_all_commands(&commands, &command_count);
    
    if (get_cmds_result == 0) {
        printf("Found %d available commands:\n", command_count);
        for (int i = 0; i < command_count && i < 10; i++) {  // Show first 10 commands
            if (commands[i]) {
                printf("  %s\n", commands[i]);
            }
        }
        if (command_count > 10) {
            printf("  ... and %d more commands\n", command_count - 10);
        }
        
        // Clean up command list
        winux_free_commands_array(commands, command_count);
    } else {
        printf("Failed to get commands (error code: %d)\n", get_cmds_result);
    }
    printf("\n");

    if (!daemon_available) {
        printf("Error: Daemon is not available. Cannot proceed with FFI test.\n");
        return 1;
    }

    // Test commands - sample of implemented commands
    const char* test_commands[] = {
        "pwd",
        "ls",
        "date",
        "whoami",
        "hostname",
        "env",
        "true",
        "false"
    };

    const char* test_args[] = {
        NULL,  // pwd
        NULL,  // ls
        NULL,  // date
        NULL,  // whoami
        NULL,  // hostname
        NULL,  // env
        NULL,  // true
        NULL   // false
    };
    int num_commands = sizeof(test_commands) / sizeof(test_commands[0]);
    int iterations = 1;  // Run each command once for debugging

    PerformanceStats stats;
    init_stats(&stats);

    printf("Starting performance test...\n");
    printf("Commands: %d, Iterations per command: %d\n\n", num_commands, iterations);

    // Execute commands
    for (int cmd_idx = 0; cmd_idx < num_commands; cmd_idx++) {
        const char* command = test_commands[cmd_idx];
        const char* args[2] = {test_args[cmd_idx], NULL};
        int arg_count = (test_args[cmd_idx] != NULL) ? 1 : 0;

        printf("Testing command: %s", command);
        if (arg_count > 0) {
            printf(" %s", args[0]);
        }
        printf("\n");

        for (int iter = 0; iter < iterations; iter++) {
            double start_time = get_time_ms();

            char* output = NULL;
            char* error = NULL;
            size_t output_size = 0;
            size_t error_size = 0;

            int exit_code = winux_execute(command, args, arg_count, NULL,
                                          &output, &error,
                                          &output_size, &error_size);

            double end_time = get_time_ms();
            double elapsed_ms = end_time - start_time;

            int success = (exit_code == 0);
            update_stats(&stats, elapsed_ms, success);

            printf("  Iteration %2d: %6.2f ms, Exit Code: %3d",
                   iter + 1, elapsed_ms, exit_code);

            if (success && output && output_size > 0) {
                // Print complete output directly (preserves ANSI color codes)
                printf(", Output:\n");
                fwrite(output, 1, output_size, stdout);
            }

            if (error && error_size > 0) {
                printf(", Error: \"");
                int preview_len = (error_size > 30) ? 30 : (int)error_size;
                for (int i = 0; i < preview_len; i++) {
                    if (error[i] >= 32 && error[i] <= 126) {
                        printf("%c", error[i]);
                    }
                }
                if (error_size > 30) printf("...");
                printf("\"");
            }

            printf("\n");

            // Free buffers
            if (output) winux_free_buffer(output);
            if (error) winux_free_buffer(error);
        }

        printf("\n");
    }

    // Print final statistics
    print_stats(&stats);

    printf("Test completed successfully!\n");
    printf("Note: This demonstrates the FFI API's ability to execute commands\n");
    printf("      with minimal overhead by reusing a persistent daemon process.\n");

    return 0;
}
