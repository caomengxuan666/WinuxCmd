export module core:pipeline;

import std;
import utils;

export namespace core::pipeline {
    using Error = std::string_view;

    template<typename T>
    using Result = std::expected<T, Error>;

    template<typename T>
    void report_error(const Result<T>& result, std::wstring_view command_name) {
        if (!result) {
            const auto& error_msg = result.error();
            std::wstring wmsg(error_msg.begin(), error_msg.end());
            safeErrorPrint(std::wstring(command_name) + L": " + wmsg + L"\n");
        }
    }

    template<typename T>
    int report_error_with_code(const Result<T>& result,
                               std::wstring_view command_name,
                               int error_code = 1) {
        if (!result) {
            report_error(result, command_name);
            return error_code;
        }
        return 0;
    }

    inline void report_custom_error(std::wstring_view command_name,
                                    std::wstring_view error_message) {
        safeErrorPrint(std::wstring(command_name) + L": " + std::wstring(error_message) + L"\n");
    }

}