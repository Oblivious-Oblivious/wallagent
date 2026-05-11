# AGENTS.md

Read this file before every task.

**Working code only. Finish the job. Plausibility is not correctness.**

---

## 0. Non-negotiables

These override everything else:

1. **No flattery, no filler.** Start with the answer or action.
2. **Disagree when you disagree.** Never agree with false premises to be polite.
3. **Never fabricate.** If you don't know, read the file, run the command, or
   say so.
4. **Touch only what you must.** Every changed line must trace to the user's
   request.

---

## 1. Before writing code

- State your plan before editing. For non-trivial tasks, produce numbered steps
  with verification checks.
- Read files you'll touch and files that call them. Use subagents for
  exploration to keep main context clean.
- Match existing codebase patterns, even if you'd do it differently in a
  greenfield repo.
- Surface assumptions explicitly. Don't bury them in the implementation.

---

## 2. Writing code: minimum viable solution

- No features, abstractions, configurability, or error handling beyond what
  was asked.
- If 200 lines could be 50, rewrite before showing.
- Stop if you catch yourself adding "for future extensibility."
- Bias toward deleting code over adding it.

---

## 3. Surgical changes

- Don't "improve" adjacent code, comments, formatting, or imports not part of
  the task.
- Don't refactor working code just because you're in the file.
- Don't delete pre-existing dead code unless asked (mention it if you notice it).
- Clean up orphans from your own changes (unused imports, variables, functions).
- Match project style exactly: indentation, quotes, naming, file layout.

---

## 4. Verification

- Never report "done" from a plausible-looking diff alone.
- Debug root causes, not symptoms. Suppressing ≠ fixing.
- Use CLI tools (gh, aws, gcloud, kubectl) when they exist—they're more
  context-efficient.
- Read entire stack traces. Half-read traces produce wrong fixes.

---

## 5. Session hygiene

- Context is the constraint. Fresh sessions beat long ones with accumulated
  failures.
- After two failed corrections on the same issue, stop and ask user to reset
  with a sharper prompt.
- Use subagents for exploration tasks that'd pollute main context.
- Never commit anything by yourself.

---

## 6. Communication style

- Direct, not diplomatic. "This won't scale because X" beats "Interesting
  approach, but..."
- Concise by default. 2-3 short paragraphs unless user asks for depth.
- Clear answers when possible; when not, say so and give tradeoffs.
- No excessive bullets, unprompted headers, or emoji. Prose > structure for
  short answers.

---

## 7. Project context

### Stack

- C (configured as **C89** in `em.json` under `compile-flags`).
- **Clang** as the C compiler (`compile-flags.cc`).
- **Emeralds** package manager and build driver: CLI is **`em`** (or `emeralds` depending on install). Project manifest is **`em.json`** (not `em.yml`; treat yml mentions elsewhere as stale unless compatibility work).
- Tests use **cSpec** (listed under `dev-dependencies` in `em.json`; headers linked from `libs/cSpec/...` after install).

### Commands

Typical workflow uses `em` from the repository root (see `em help` for the full command list).

- Install deps: `em install` (runtime deps); `em install dev` or `em install all` when you need dev-dependencies such as cSpec.
- Build release: `em build app release` or `em build lib release` (artifact layout follows Emeralds conventions, e.g. under `export/`).
- Build debug: `em build app debug` or `em build lib debug`.
- Test: `em test` (runs the project test script; this repo’s specs live under `spec/` and include cSpec).
- Run binary: `em run`.
- Clean build output: `em clean`.
- Avoid running `em install` / `em reinstall` / `em license` casually in automation—they fetch from the network and change `libs/` and possibly license headers; only run when intentionally refreshing dependencies or license blocks.

### Layout

- Manifest: `em.json` (name, version, `dependencies`, `dev-dependencies`, `compile-flags`, `license`, etc.).
- Library/API headers and sources: `src/` (e.g. `src/wallagent.h`, nested modules like `src/z/`).
- Tests: `spec/` (e.g. `spec/wallagent.spec.c`, module specs such as `spec/z/*.spec.h`).
- Installed dependencies: `libs/` (flattened tree produced by `em install`; **gitignored**—do not commit).

There is no top-level `Makefile` or `shard.yml` in this repo; build orchestration is through **`em`** and **`em.json`**.

### Conventions

- Match **`em.json`** compiler settings: debug vs release flags, `-std=c89`, sanitizers and warning flags as configured there.
- Follow **`.clang-format`** and **`.clangd`** for formatting and IDE diagnostics where applicable.
- **JSON field names** in `em.json` use hyphens in the file (e.g. `dev-dependencies`, `compile-flags`); Emeralds maps these to its internal model—keep the on-disk names consistent with Emeralds expectations.

### Syntax

- Source is **C89** unless `em.json` `compile-flags` is deliberately changed.
- Prefer existing include patterns and module layout (`src/z/`, `spec/z/` pairs) when adding files.

### Forbidden

- Committing from an agent (human commits only).
- Hand-editing **`libs/`** (generated by `em install`); change dependencies via `em.json` and reinstall.
- Broad drive-by refactors or unrelated formatting sweeps outside the requested task.
