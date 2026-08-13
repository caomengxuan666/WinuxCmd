# WinuxCmd I18N workflow

WinuxCmd uses English as the built-in fallback. A language is enabled only
when `WINUX_LANG` selects a catalog under `.wpm/i18n/<locale>/catalog.json`.

# How to build 
- Always use the scripts/build-with-vs.ps1 to build the project.
- Remember use scripts/format.py or scripts/format.sh to format the files.

## Source changes

- Every user-visible natural-language message needs a stable I18N key and an
  English fallback in the source.
- Keep command names, options, paths, URLs, hashes, protocol fields, numeric
  values, shell syntax, and user file contents unchanged.
- Use `scripts/i18n_batch.py extract` to regenerate the English catalog after
  adding commands, options, help text, or runtime messages.
- Use `scripts/i18n_batch.py validate` before publishing a locale catalog.

## Catalog ownership

Catalogs are published in the separate `unixwin/winuxcmd-i18n` repository.
Local catalogs may be generated under `.wpm/i18n/` for testing, but must not be
committed to this repository. Use `scripts/i18n_sync.py` to prepare or publish
catalog updates.

When a command, option, version, or user-visible message changes, update the
catalog before the WinuxCmd release. The release workflow publishes the
English catalog automatically and preserves locale catalogs from the I18N
repository. Translation batches and manual review happen independently of the
WinuxCmd binary build.
