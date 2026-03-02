# Session Archiving Instruction

**Trigger Phrase:** `update context and archive session`

**Action:** 
1. Use the file creation tool to generate a new file at `.github/instructions.md/vsc-context/<YYYY-MM-DD>.vsc-context.md`. (Use the actual target timestamp replacing `<YYYY-MM-DD>`).
2. Reply to the user with a reminder: *"Summary created! ⚠️ Don't forget to click `...` > `Export Session` in the chat panel and save it as `.github/instructions.md/vsc-context/agent-sessions/<YYYY-MM-DD>.json`."*

**Content Template Requirement:**
Populate the file strictly using this structure:

```markdown
# Session Context Archive: <Date>

[Raw Session Log](./agent-sessions/<YYYY-MM-DD>.json)

## Conversation Summary
* <Bullet points of key decisions, goals achieved, and custom instructions provided>
* <Summary of files modified or logic updated>

## Environment & Development Context
* <Current focus areas in the workspace>
* <Last known terminal commands or system tools used (e.g., zenity, kdialog)>
* <Any encountered bugs or roadblocks>
```