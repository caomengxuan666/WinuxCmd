---
name: ffmpeg-master
version: 1.0.0
description: Use when performing video/audio processing tasks including transcoding, filtering, streaming, metadata manipulation, or complex filtergraph operations with FFmpeg.
triggers:
  - ffmpeg
  - ffprobe
  - video processing
  - audio conversion
  - codec
  - transcoding
  - filter_complex
  - h264
  - h265
  - mp4
  - mkv
  - hardware acceleration
role: specialist
scope: implementation
output-format: shell-command
---

# FFmpeg Master

Comprehensive guide for professional video and audio manipulation using FFmpeg and FFprobe.

## Core Concepts

FFmpeg is the leading multimedia framework, able to **decode, encode, transcode, mux, demux, stream, filter and play** almost anything that humans and machines have created. It is a command-line tool that processes streams through a complex pipeline of demuxers, decoders, filters, encoders, and muxers.

## Common Operations

```bash
# Basic Transcoding (MP4 to MKV)
ffmpeg -i input.mp4 output.mkv

# Change Video Codec (to H.265/HEVC)
ffmpeg -i input.mp4 -c:v libx265 -crf 28 -c:a copy output.mp4

# Extract Audio (No Video)
ffmpeg -i input.mp4 -vn -c:a libmp3lame -q:a 2 output.mp3

# Resize/Scale Video
ffmpeg -i input.mp4 -vf "scale=1280:720" output.mp4

# Cut Video (Start at 10s, Duration 30s)
ffmpeg -i input.mp4 -ss 00:00:10 -t 00:00:30 -c copy output.mp4

# Fast Precise Cut (Re-encoding only the cut points is complex, so standard re-encoding is safer for precision)
ffmpeg -ss 00:00:10 -i input.mp4 -to 00:00:40 -c:v libx264 -crf 23 -c:a aac output.mp4

# Concatenate Files (using demuxer)
# Create filelist.txt: file 'part1.mp4' \n file 'part2.mp4'
ffmpeg -f concat -safe 0 -i filelist.txt -c copy output.mp4

# Speed Up/Slow Down Video (2x speed)
ffmpeg -i input.mp4 -filter_complex "[0:v]setpts=0.5*PTS[v];[0:a]atempo=2.0[a]" -map "[v]" -map "[a]" output.mp4
```

---

## Processing Categories & When to Use

### Codecs & Quality
| Option | Use When |
|-----------|----------|
| `-c:v libx264` | Standard H.264 encoding (best compatibility) |
| `-c:v libx265` | H.265/HEVC encoding (best compression/quality) |
| `-crf [0-51]` | Constant Rate Factor (lower is higher quality, 18-28 recommended) |
| `-preset` | Encoding speed vs compression (ultrafast, medium, veryslow) |
| `-c:a copy` | Pass-through audio without re-encoding (saves time/quality) |

### Filters & Manipulation
| Filter | Use When |
|-----------|----------|
| `scale` | Changing resolution (e.g., `scale=1920:-1` for 1080p width) |
| `crop` | Removing edges (e.g., `crop=w:h:x:y`) |
| `transpose` | Rotating video (1=90deg CW, 2=90deg CCW) |
| `fps` | Changing frame rate (e.g., `fps=30`) |
| `drawtext` | Adding text overlays/watermarks |
| `overlay` | Picture-in-picture or adding image watermarks |
| `fade` | Adding fade-in/out effects (e.g., `fade=in:0:30` for first 30 frames) |
| `volume` | Adjusting audio levels (e.g., `volume=1.5` for 150% volume) |
| `setpts` | Changing video speed (e.g., `setpts=0.5*PTS` for double speed) |
| `atempo` | Changing audio speed without pitch shift (0.5 to 2.0) |

### Inspection & Metadata
| Tool/Option | Use When |
|-----------|----------|
| `ffprobe -v error -show_format -show_streams` | Getting detailed technical info of a file |
| `-metadata title="Name"` | Setting global metadata tags |
| `-map` | Selecting specific streams (e.g., `-map 0:v:0 -map 0:a:1`) |

---

## Advanced: Complex Filtergraphs

Use `filter_complex` when you need to process multiple inputs or create non-linear filter chains.

```bash
# Example: Adding a watermark at the bottom right
ffmpeg -i input.mp4 -i watermark.png -filter_complex "overlay=main_w-overlay_w-10:main_h-overlay_h-10" output.mp4

# Example: Vertical Stack (2 videos)
ffmpeg -i top.mp4 -i bottom.mp4 -filter_complex "vstack=inputs=2" output.mp4

# Example: Side-by-Side (2 videos)
ffmpeg -i left.mp4 -i right.mp4 -filter_complex "hstack=inputs=2" output.mp4

# Example: Grid (4 videos 2x2)
ffmpeg -i v1.mp4 -i v2.mp4 -i v3.mp4 -i v4.mp4 -filter_complex "[0:v][1:v]hstack=inputs=2[top];[2:v][3:v]hstack=inputs=2[bottom];[top][bottom]vstack=inputs=2" output.mp4

# Example: Fade Transition (Simple crossfade between two clips)
# Requires manual offset calculation, using xfade is better
ffmpeg -i input1.mp4 -i input2.mp4 -filter_complex "xfade=transition=fade:duration=1:offset=9" output.mp4
```

## Hardware Acceleration

| Platform | Codec | Command |
|----------|-------|---------|
| NVIDIA (NVENC) | H.264 | `-c:v h264_nvenc` |
| Intel (QSV) | H.264 | `-c:v h264_qsv` |
| Apple (VideoToolbox) | H.265 | `-c:v hevc_videotoolbox` |

## Constraints & Error Handling

- **Stream Mapping**: Always use `-map` for complex files to ensure you get the right audio/subtitle tracks.
- **Seeking**: Put `-ss` *before* `-i` for fast seeking (input seeking), or *after* `-i` for accurate seeking (output seeking).
- **Format Support**: Ensure the output container (extension) supports the codecs you've chosen.
