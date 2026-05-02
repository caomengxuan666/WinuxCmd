---
name: ai-review-assistant
description: Reads content from URLs or files, classifies it, and generates structured summaries and comments in a specific, analytical style. Use when you need to process articles, papers, or news and create a formatted review with personal commentary.
license: Complete terms in LICENSE.txt
---

# AI Review Assistant

This skill enables Manus to act as a strategic AI analyst. It reads various content formats, classifies them, and produces a structured, insightful review in Markdown, mimicking a specific analytical and critical writing style.

## Core Workflow

The process involves these sequential steps. Follow them in order to generate the review.

1.  **Receive Input:** The user will provide content via a URL or a file path (`.pdf`, `.md`, `.txt`).
2.  **Read Content:** Use the appropriate tool to read the content. Refer to the **Content Reading Guide** below to handle different sources.
3.  **Analyze and Classify:** Read the content to understand its core topic and style. Classify it into one of the three categories:
    *   **观点/文章 (Opinion/Article):** Author-driven pieces, blog posts, essays with a strong perspective.
    *   **Solid Paper:** Technical or research papers with a formal structure.
    *   **行业动态/评价 (Industry News/Commentary):** News, announcements, or market analysis.
4.  **Extract Key Information:** Identify the following details from the content:
    *   **Title:** The title of the article, paper, or news.
    *   **URL/Source:** The original URL or file name.
    *   **Date:** The publication date of the content (format: YYYY-MM).
    *   **Key Takeaway (为什么值得关注):** A concise, one-sentence summary of why the content is important or interesting.
5.  **Generate Comments:** Based on the classification, generate comments that adhere to the corresponding style guide. This is the most critical step.
    *   For **观点/文章**, read `/home/ubuntu/skills/ai-review-assistant/references/style_guide_article.md`.
    *   For **Solid Paper**, read `/home/ubuntu/skills/ai-review-assistant/references/style_guide_paper.md`.
    *   For **行业动态/评价**, read `/home/ubuntu/skills/ai-review-assistant/references/style_guide_industry.md`.
6.  **Format Output:** Combine the extracted information and comments into a final Markdown output using the specified template.

## Content Reading Guide

Based on the input type, use the following strategy to read the content. If a method fails, clearly inform the user that the content could not be read.

| Content Source          | Recommended Reading Strategy                                                                                             |
| ----------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| **Web URLs**            | Use `browser_navigate`. If initial Markdown extraction is incomplete, use `browser_scroll` to load the full page.       |
| **PDF Files (URL/Local)** | Use `shell` with `curl` to download and `pdftotext` to extract text (e.g., `curl -o t.pdf <URL> && pdftotext t.pdf -`). |
| **YouTube Videos**      | Use `browser_navigate`. The system will automatically provide a summary. Read the generated summary file.                |
| **Podcast Webpages**    | Use `browser_navigate` to get Show Notes. For audio, use `manus-speech-to-text` if an audio file is available.        |
| **Local Files (.md/.txt)** | Use `file` tool with the `read` action.                                                                                  |

## Output Template

ALWAYS use this exact Markdown template for the final output. Do not add any extra titles or introductions.

```markdown
| Type | Date | Title | URL/Source | Why it's worth watching | Comments |
|---|---|---|---|---|---|
| **[分类]** | [发布时间] | [文章标题] | [链接/来源] | [为什么值得关注] | [生成的评论] |
```

Replace the bracketed placeholders `[]` with the information you have gathered and the comments you have generated.
