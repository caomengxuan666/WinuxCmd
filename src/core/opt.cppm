export module core:opt;

import std;
import :cmd_meta;

using OptionValue = std::variant<
    bool,
    int,
    std::string
>;

// Option handler structure for lookup table
export struct OptionHandler {
    char short_opt;
    bool requires_arg = false;
    const char *long_opt = nullptr;
};

// Helper functions for option handling
export constexpr char get_short_opt(std::string_view short_name) {
    if (short_name.size() >= 2 && short_name[0] == '-') {
        return short_name[1];
    }
    return 0;
}

// Helper function to check if an option requires an argument
export constexpr bool requires_argument(std::string_view long_name) {
    // Common options that require arguments
    return false;
}

// Overload that accepts a custom set of argument-requiring options
export template<typename... Args>
constexpr bool requires_argument(std::string_view long_name, Args... args) {
    // Check if long_name matches any of the argument-requiring options
    return ((long_name == args) || ...);
}

export constexpr const char *sv_to_cstr(std::string_view sv) {
    return sv.empty() ? nullptr : sv.data();
}

// Metafunction to generate OPTION_HANDLERS from options array
template<size_t N, size_t... Is, typename... Args>
constexpr auto generate_option_handlers_impl(
    const std::array<cmd::meta::OptionMeta, N> &options,
    std::index_sequence<Is...>, Args... args) {
    return std::array<OptionHandler, N>{
        {
            {
                get_short_opt(options[Is].short_name),
                requires_argument(options[Is].long_name, args...),
                sv_to_cstr(options[Is].long_name)
            }...
        }
    };
}

// Base case: no argument-requiring options
export template<size_t N>
constexpr auto generate_option_handlers(
    const std::array<cmd::meta::OptionMeta, N> &options) {
    return generate_option_handlers_impl(options, std::make_index_sequence<N>{});
}

// Overload: accept custom argument-requiring options
export template<size_t N, typename... Args>
constexpr auto generate_option_handlers(
    const std::array<cmd::meta::OptionMeta, N> &options, Args... args) {
    return generate_option_handlers_impl(options, std::make_index_sequence<N>{},
                                         args...);
}


export class ParsedOptions {
private:
    std::unordered_map<std::string_view, OptionValue> m_values;

public:
    void set(std::string_view long_name, OptionValue value) {
        m_values[long_name] = std::move(value);
    }

    bool has(std::string_view long_name) const {
        return m_values.find(long_name) != m_values.end();
    }

    template<typename T>
    T get(std::string_view long_name, T default_value = {}) const {
        auto it = m_values.find(long_name);
        if (it == m_values.end())
            return default_value;

        if (auto p = std::get_if<T>(&it->second))
            return *p;

        return default_value;
    }
};

export template<size_t N>
bool parse_long_options(
    std::span<std::string_view> args,
    const std::array<cmd::meta::OptionMeta, N> &metas,
    ParsedOptions &out) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string_view arg = args[i];

        if (!arg.starts_with("--"))
            continue;

        const cmd::meta::OptionMeta *meta = nullptr;
        for (const auto &m: metas) {
            if (m.long_name == arg) {
                meta = &m;
                break;
            }
        }

        if (!meta)
            return false; // unknown option

        using cmd::meta::OptionType;

        switch (meta->type) {
            case OptionType::Bool:
                out.set(meta->long_name, true);
                break;

            case OptionType::Int:
                if (i + 1 >= args.size())
                    return false;
                try {
                    int v = std::stoi(std::string(args[++i]));
                    out.set(meta->long_name, v);
                } catch (...) {
                    return false;
                }
                break;

            case OptionType::String:
                if (i + 1 >= args.size())
                    return false;
                out.set(meta->long_name, std::string(args[++i]));
                break;
        }
    }

    return true;
}

export struct ParseResult {
    ParsedOptions options;
    std::vector<std::string_view> positionals;
    bool ok = true;
};

inline std::string_view option_key(const cmd::meta::OptionMeta* m) {
    if (!m->long_name.empty())
        return m->long_name;
    return m->short_name;
}

export ParseResult parse_command(
    std::span<std::string_view> args,
    std::span<const cmd::meta::OptionMeta> metas) {
    ParseResult result{};
    using cmd::meta::OptionType;

    bool end_of_options = false;

    for (size_t i = 0; i < args.size(); ++i) {
        std::string_view arg = args[i];

        // ---------- "--" : end of options ----------
        if (!end_of_options && arg == "--") {
            end_of_options = true;
            continue;
        }

        // ---------- long option ----------
        if (!end_of_options && arg.starts_with("--")) {
            const cmd::meta::OptionMeta *meta = nullptr;

            for (const auto &m: metas) {
                if (m.long_name == arg) {
                    meta = &m;
                    break;
                }
            }

            if (!meta) {
                result.ok = false;
                return result;
            }

            switch (meta->type) {
                case OptionType::Bool:
                    result.options.set(option_key(meta), true);
                    break;

                case OptionType::Int: {
                    if (i + 1 >= args.size()) {
                        result.ok = false;
                        return result;
                    }

                    try {
                        int v = std::stoi(std::string(args[++i]));
                        result.options.set(option_key(meta), v);
                    } catch (...) {
                        result.ok = false;
                        return result;
                    }
                    break;
                }

                case OptionType::String: {
                    if (i + 1 >= args.size()) {
                        result.ok = false;
                        return result;
                    }

                    result.options.set(option_key(meta), std::string(args[++i]));
                    break;
                }
            }

            continue;
        }

        // ---------- short option(s) ----------
        if (!end_of_options &&
            arg.size() >= 2 &&
            arg[0] == '-' &&
            arg[1] != '-') {
            // iterate each short flag: -abc
            for (size_t pos = 1; pos < arg.size(); ++pos) {
                char ch = arg[pos];

                const cmd::meta::OptionMeta *meta = nullptr;

                for (const auto &m: metas) {
                    if (!m.short_name.empty() &&
                        m.short_name.size() == 2 &&
                        m.short_name[0] == '-' &&
                        m.short_name[1] == ch) {
                        meta = &m;
                        break;
                    }
                }

                if (!meta) {
                    result.ok = false;
                    return result;
                }

                switch (meta->type) {
                    // ----- bool: can be grouped -----
                    case OptionType::Bool:
                        result.options.set(meta->long_name, true);
                        break;

                    // ----- value option -----
                    case OptionType::Int:
                    case OptionType::String: {
                        // value option MUST be last in group
                        if (pos != arg.size() - 1) {
                            result.ok = false;
                            return result;
                        }

                        if (i + 1 >= args.size()) {
                            result.ok = false;
                            return result;
                        }

                        if (meta->type == OptionType::Int) {
                            try {
                                int v = std::stoi(std::string(args[++i]));
                                result.options.set(meta->long_name, v);
                            } catch (...) {
                                result.ok = false;
                                return result;
                            }
                        } else {
                            result.options.set(meta->long_name,
                                               std::string(args[++i]));
                        }

                        break;
                    }
                }
            }

            continue;
        }

        // ---------- positional ----------
        result.positionals.push_back(arg);
    }

    return result;
}
