---
name: agent-wal
description: "Write-Ahead Log protocol for agent state persistence. Prevents losing corrections, decisions, and context during conversation compaction. Use when: (1) receiving a user correction — log it before responding, (2) making an important decision or analysis — log it before continuing, (3) pre-compaction memory flush — flush the working buffer to WAL, (4) session start — replay unapplied WAL entries to restore lost context, (5) any time you want to ensure something survives compaction."
---

# Agent WAL (Write-Ahead Log)

Write important state to disk **before** responding. Prevents the #1 agent failure mode: losing corrections and context during compaction.

## Core Rule

**Write before you respond.** If something is worth remembering, WAL it first.

## When to WAL

| Trigger | Action Type | Example |
|---------|------------|---------|
| User corrects you | `correction` | "No, use Podman not Docker" |
| You make a key decision | `decision` | "Using CogVideoX-2B for text-to-video" |
| Important analysis/conclusion | `analysis` | "WAL/VFM patterns should be core infra not skills" |
| State change | `state_change` | "GPU server SSH key auth configured" |
| User says "remember this" | `correction` | Whatever they said |

## Commands

All commands via `scripts/wal.py` (relative to this skill directory):

```bash
# Write before responding
python3 scripts/wal.py append agent1 correction "Use Podman not Docker for all EvoClaw tooling"
python3 scripts/wal.py append agent1 decision "CogVideoX-5B with multi-GPU via accelerate"
python3 scripts/wal.py append agent1 analysis "Signed constraints prevent genome tampering"

# Working buffer (batch writes during conversation, flush before compaction)
python3 scripts/wal.py buffer-add agent1 decision "Some decision"
python3 scripts/wal.py flush-buffer agent1

# Session start: replay lost context
python3 scripts/wal.py replay agent1

# After applying a replayed entry
python3 scripts/wal.py mark-applied agent1 <entry_id>

# Maintenance
python3 scripts/wal.py status agent1
python3 scripts/wal.py prune agent1 --keep 50
```

## Integration Points

### On Session Start
1. Run `replay` to get unapplied entries
2. Read the summary into your context
3. Mark entries as applied after incorporating them

### On User Correction
1. Run `append` with action_type `correction` BEFORE responding
2. Then respond with the corrected behavior

### On Pre-Compaction Flush
1. Run `flush-buffer` to persist any buffered entries
2. Then write to daily memory files as usual

### During Conversation
For less critical items, use `buffer-add` to batch writes. Buffer is flushed to WAL on `flush-buffer` (called during pre-compaction) or manually.

## Storage

WAL files: `~/clawd/memory/wal/<agent_id>.wal.jsonl`
Buffer files: `~/clawd/memory/wal/<agent_id>.buffer.jsonl`

Entries are append-only JSONL. Each entry:
```json
{"id": "abc123", "timestamp": "ISO8601", "agent_id": "agent1", "action_type": "correction", "payload": "Use Podman not Docker", "applied": false}
```
