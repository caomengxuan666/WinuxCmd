export module core:command_context;
import :opt;


export struct CommandContext {
    ParsedOptions options;
    std::vector<std::string_view> positionals;
};

export CommandContext make_context(
    std::span<std::string_view> args,
    std::span<const cmd::meta::OptionMeta> metas,
    bool& ok)
{
    auto res = parse_command(args, metas);
    ok = res.ok;

    CommandContext ctx;
    ctx.options = std::move(res.options);
    ctx.positionals = std::move(res.positionals);
    return ctx;
}