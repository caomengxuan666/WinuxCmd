---
name: briefing
description: "Daily briefing: gathers calendar (gcalcli-calendar), active todos (todo-management), and weather (openmeteo-sh-weather-simple) into a concise summary."
metadata: {"openclaw":{"emoji":"📋"}}
user-invocable: true
---

# Briefing

Compose a daily briefing using companion skills. Each source is optional — skip it if the skill is not available.

## Sources

Three companion skills. Skip any that are not in the available skills list:

- **Calendar** — `gcalcli-calendar`
- **Todos** — `todo-management`
- **Weather** — `openmeteo-sh-weather-simple` (requires a default city/country in session context)

If **none** of the three are available — tell the user you have nothing to build a briefing from and stop. Do not fabricate a briefing.

## Briefing day

Decide whether the user's day is effectively over based on current time and today's remaining calendar events.

- Day still active -> briefing covers **today**.
- Day winding down -> briefing covers **tomorrow**.

Todos are not date-bound — always show active items.

## Gather

For each available source, **read its `SKILL.md` before calling any commands**.

### Calendar

1. Read `gcalcli-calendar` SKILL.md.
2. Fetch the **briefing day's** agenda.
3. If no events — note it.

### Todos

1. Read `todo-management` SKILL.md.
2. List **active/pending** items.
3. If none — note it.

### Weather

1. Get the **briefing day's** conditions and forecast. Use the user's default city/country from session context.
2. Commands (always use `--llm`):
   - Today: `openmeteo weather --current --forecast-days=1 --city="{city}" --llm`
   - Tomorrow: `openmeteo weather --current --forecast-days=2 --forecast-since=2 --city="{city}" --llm`

### On errors

If a command fails, skip that section and mention the failure briefly. Do not retry more than once. Never fabricate data.

## Compose

Build a single message. Include only sections whose skill was available. If a skill returned no data, still include the section with a one-line note.

### Structure

1. **Title line** — compact: date, day-of-week, time. E.g. `Брифинг 14.02 (пт, 8:12)`. If briefing day is tomorrow, say so.
2. **Weather** — 1–2 lines: temperature, sky, anything notable.
3. **Calendar** — briefing day's events, chronologically. Format: `HH:MM — Title`. All-day events first. If empty: one line noting no events.
4. **Upcoming** — next 2-3 days' notable events (if any), one line per day. Omit if nothing notable.
5. **Todos** — active items, briefly. Higher priority first if supported. If empty: one line noting no todos.

### Example output

Follow this format exactly in the user's language:
```
Briefing 14.02 (Sat, 8:12)

**🌤 Weather (London, UK)**
+2°C, cloudy, wind 11 km/h. Daytime to -3°C, light rain.

**📅 Calendar**
09:00 — Standup
14:00 — Sprint review
18:30 — Driving school

**🔜 Upcoming**
• 15.02: Free day.
• 16.02: Daily standup 12:00, Driving school 18:30.

**✅ Todos**
• [work] Debug feature X.
• [personal] Book a doctor's appointment.
```
Note: bold header → content immediately on next line (zero blank lines); one blank line between sections; no trailing question or CTA.

### Formatting rules (strict)

These rules are critical for readability on mobile. Follow them exactly.

- **Between sections**: exactly one empty line.
- **Between a section header and its content**: zero empty lines. Content starts on the very next line.
- Right:
```
**Calendar**
09:00 — Standup
14:00 — Review
```
- Wrong:
```
**Calendar**

09:00 — Standup
14:00 — Review
```
- When briefing day is tomorrow, calendar and weather headers should reflect that.
- Do not shorten the user's city name.
- Match the language of the user's request.
- Simple formatting — optimize for mobile chat. Bold section headers, short lines.
- Concise, skimmable, no filler.

### Strict prohibitions

- No preamble — dive straight in.
- No call to action or question at the end. The briefing ends after the last section. No "What's next?", "What's first?", or similar.
- Never invent events, todos, or weather data. Only report what tools returned.
