# winuxsh-native-codex

Native Winuxsh execution guidance for Codex. This skill keeps Codex inside the current Winuxsh session and preserves native Windows path semantics.

## Scope

- Run ordinary shell and development commands directly in Winuxsh.
- Use native Windows paths such as C:/Users/... instead of MSYS, WSL, or /c/... paths.
- Configure .winuxshrc, aliases, PATH entries, oh-my-winuxsh, and .winux plugins.
- Validate shell changes and executable availability without blindly sourcing unsafe rc files.

## Installation

Place this directory at:

    ~/.codex/skills/winuxsh-native-codex

The required file is SKILL.md. Codex can discover the skill from its frontmatter name and description.

## Validation

Run the skill-creator validator:

    python scripts/quick_validate.py ~/.codex/skills/winuxsh-native-codex

## Package Contents

- SKILL.md: Codex instructions and triggering metadata.
- README.md: Human-readable overview and installation notes.
