/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: ffi.h
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */

#ifndef WINUX_FFI_H
#define WINUX_FFI_H

#ifdef _WIN32
#ifdef WINUX_FFI_EXPORTS
#define WINUX_API __declspec(dllexport)
#else
#define WINUX_API __declspec(dllimport)
#endif
#else
#define WINUX_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize daemon mode (optional, daemon will auto-start on first
 * execution)
 * @return 0 on success, non-zero on error
 *
 * This function is optional. The daemon will be automatically started
 * on the first call to winux_execute() if not already running.
 */
WINUX_API int winux_init_daemon(void);

/**
 * @brief Execute command via daemon (zero start-up overhead)
 *
 * @param command Command name (e.g., "ls", "echo")
 * @param args Array of arguments (NULL-terminated, can be NULL)
 * @param arg_count Number of arguments
 * @param cwd Current working directory (NULL to use current directory)
 * @param output Output buffer (allocated by FFI, must be freed with
 * winux_free_buffer)
 * @param error Error buffer (allocated by FFI, must be freed with
 * winux_free_buffer)
 * @param output_size Output size in bytes
 * @param error_size Error size in bytes
 * @return Exit code (0 on success, non-zero on error)
 *
 * @note All output and error buffers are allocated by the FFI and must be
 *       freed using winux_free_buffer() to avoid memory leaks.
 *
 * Example:
 * @code
 * const char* args[] = {"-la"};
 * char* output = NULL;
 * char* error = NULL;
 * size_t out_size = 0, err_size = 0;
 *
 * int exit_code = winux_execute("ls", args, 1, NULL,
 *     &output, &error, &out_size, &err_size);
 *
 * if (output) {
 *     printf("%.*s", (int)out_size, output);
 *     winux_free_buffer(output);
 * }
 *
 * if (error) {
 *     fprintf(stderr, "%.*s", (int)err_size, error);
 *     winux_free_buffer(error);
 * }
 * @endcode
 */
WINUX_API int winux_execute(const char* command, const char** args,
                            int arg_count, const char* cwd, char** output,
                            char** error, size_t* output_size,
                            size_t* error_size);

/**
 * @brief Free memory allocated by FFI functions
 * @param buffer Buffer to free (can be NULL)
 *
 * Must be called for all output and error buffers returned by winux_execute().
 */
WINUX_API void winux_free_buffer(char* buffer);

/**
 * @brief Check if daemon is available
 * @return 1 if daemon is available, 0 otherwise
 */
WINUX_API int winux_is_daemon_available(void);

/**
 * @brief Get version information
 * @return Version string (e.g., "0.7.2")
 *
 * The returned string is statically allocated and must not be freed.
 */
WINUX_API const char* winux_get_version(void);

/**
 * @brief Get protocol version
 * @return Protocol version number (e.g., 1)
 */
WINUX_API int winux_get_protocol_version(void);

#ifdef __cplusplus
}
#endif

#endif  // WINUX_FFI_H
