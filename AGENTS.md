# AGENTS.md

Behavioral guidelines to reduce common LLM coding mistakes. Merge with
project-specific instructions as needed.

**Tradeoff:** These guidelines bias toward caution over speed. For trivial
tasks, use judgment.

## 0. Non-negotiables

These override everything else:

1. **No flattery, no filler.** Start with the answer or action.
2. **Disagree when you disagree.** Never agree with false premises to be polite.
3. **Never fabricate.** If you don't know, read the file, run the command, or
   say so.
4. **Touch only what you must.** Every changed line must trace to the user's
   request.
5. **NO committing from an agent!!** (human commits only).
6. **Do not write comments** unless they are javadoc style comments.  Even
   in that case, only write them if the project looks like a library not an app.

## 1. Think Before Coding

**Don't assume. Don't hide confusion. Surface tradeoffs.**

Before implementing:

- State your assumptions explicitly. If uncertain, ask.
- If multiple interpretations exist, present them - don't pick silently.
- If a simpler approach exists, say so. Push back when warranted.
- If something is unclear, stop. Name what's confusing. Ask.

## 2. Simplicity First

**Minimum code that solves the problem. Nothing speculative.**

- No features beyond what was asked.
- No abstractions for single-use code.
- No "flexibility" or "configurability" that wasn't requested.
- No error handling for impossible scenarios.
- If you write 200 lines and it could be 50, rewrite it.
- If you write 20 lines and it be 5, rewrite it.

Ask yourself: "Would a senior engineer say this is overcomplicated?" If yes,
simplify.

## 3. Surgical Changes

**Touch only what you must. Clean up only your own mess.**

When editing existing code:

- Don't "improve" adjacent code, comments, or formatting.
- Don't refactor things that aren't broken.
- Match existing style, even if you'd do it differently.
- If you notice unrelated dead code, mention it - don't delete it.

When your changes create orphans:

- Remove imports/variables/functions that YOUR changes made unused.
- Don't remove pre-existing dead code unless asked.

The test: Every changed line should trace directly to the user's request.

## 4. Goal-Driven Execution

**Define success criteria. Loop until verified.**

Transform tasks into verifiable goals:

- "Add validation" → "Write tests for invalid inputs, then make them pass"
- "Fix the bug" → "Write a test that reproduces it, then make it pass"
- "Refactor X" → "Ensure tests pass before and after"

For multi-step tasks, state a brief plan:

```
1. [Step] → verify: [check]
2. [Step] → verify: [check]
3. [Step] → verify: [check]
```

Strong success criteria let you loop independently. Weak criteria ("make it
work") require constant clarification.

---

**These guidelines are working if:** fewer unnecessary changes in diffs, fewer
rewrites due to overcomplication, and clarifying questions come before
implementation rather than after mistakes.

## 5. Project Context

### Stack

- C project managed by Emeralds (`em` CLI).
- Compiler config comes from `em.json`.

### Commands

- `em help` - get information about all commands. use this to navigate.

- Examples:
  - `em build [app | lib] [debug | release]` — build the project.
  - `em test` — run tests.
  - `em install` — fetch dependencies into `libs/`.
  - `em add <name>` — scaffold a new module (source + header).

### Layout

- `src/` — source modules (`.c` and `.h` files).
- `spec/` — cSpec test files.
- `libs/` — installed dependencies (gitignored).
- `export/` — dependency public headers.
- `em.json` — project configuration (single source of truth).

### Config Format

- Compile flags per platform: `compile-flags.darwin` / `linux` / `win32`.
- Each platform has `debug` and `release` profiles.
- Production dependencies: `dependencies`.
- Development dependencies: `dev-dependencies`.

### Testing

- Framework: cSpec.
- Spec files live in `spec/`.
- Include path: `libs/cSpec/export/cSpec.h`.
- Suite runner pattern: `cspec_run_suite(...)`.

### Code Style

- Favor C89 compatibility by default; only use newer C features when
  explicitly told to.
- `.clang-format` is authoritative. Don't override it.
- `.clangd` provides editor intelligence.

# cSpec — Usage Reference

Single-header, C89, compile-time TDD/BDD unit testing (RSpec-style). No linking,
no runtime deps — just `#include "libs/cSpec/export/cSpec.h"`.

Convention: one module per `<name>.module.spec.h`, plus one `*.spec.c` runner
that includes them. Build/run with `em test`.

## Example

```c
/* stack.module.spec.h */
#include "../src/cSpec.h"

module(T_stack, {              /* defines void T_stack(void) */
  before_each(&setup);         /* void(void) ptr, runs around every `it` */
  after_each(&defer);

  describe("stack", {
    int x;
    before({ x = 99; });       /* inlined ONCE here, not per-test */

    it("pops what it pushed", {
      push(s, x);
      assert_that_int(pop(s) equals to x);
    });

    after({ /* one-time teardown */ });
  });
})

/* runner.spec.c */
int main(void) {
  cspec_run_suite("all", {     /* "all" | "passing" | "failing" | "skipped" */
    T_stack();                 /* call each module */
  });
}
```

`cspec_run_suite(type, {...})`: all tests run; `type` only filters what prints.

## Structure

- `module(name, {...})` — top-level container; defines callable `name`.
- `describe("text", {...})` / `context(...)` — groups (aliases); nest freely.
- `it("text", {...})` — one test; independent; any failed assert fails it.

## Setup & teardown

- `before({...})` / `after({...})` — inlined **once** where written (block-level).
- `before_each(&fn)` / `after_each(&fn)` — `void fn(void)` run **around every `it`**.

## Assertions

```c
assert_that(expr);                  /* fail if false; nassert_that = fail if true */
assert_that(len is 0);              /* sugar: is -> ==,  isnot -> != */
fail("message");                    /* always fails */

assert_that_int(got equals to 2);   /* typed equality; nassert_that_int negates */
assert_that_charptr(s equals to ""); /* charptr compares contents */
assert_that_int_array(got equals to want with array_size 5);
```

Typed `<type>` suffixes (each has 4 forms: `assert_that_<t>`, `nassert_that_<t>`,
`..._array`, `nassert..._array`):
`char`, `unsigned_char`, `short`, `unsigned_short`, `int`, `unsigned_int`,
`long`, `unsigned_long`, `long_long`, `unsigned_long_long`, `size_t`,
`ptrdiff_t`, `void_ptr`, `float`, `double`, `long_double`, `charptr`.

Sugar words: `is`=`==`, `isnot`=`!=`, `equals`=`,`, `array_size`=`,`,
`to`/`with`=nothing. So `assert_that_int(a equals to b)` is `assert_that_int(a, b)`.
Floats compare within `1E-12`; `void_ptr` compares addresses.

## Skipping

Prefix with `x` to skip (body not run, counts as skipped): `xmodule`,
`xdescribe`, `xcontext`, `xit`. Shown only under `"all"`/`"skipped"`.

## Gotchas

- **C89**: declare locals at top of each block; no `//` comments.
- `describe` state persists across `it`s unless reset in `before`.
- `before`/`after` are one-time, not per-test — use `before_each`/`after_each`.
- Runner string must be exactly `all`/`passing`/`failing`/`skipped`, else nothing runs.
- Failures auto-report `__FILE__:__LINE__`.
