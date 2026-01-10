# DShell

## Overview
DShell is a small, modular Unix-like command shell written in C.
It supports builtin commands, basic job control, pipelines, background execution, and a simple history mechanism (`!!`).
The project is structured into clear layers (tokenization, parsing, command execution, and job management) to allow future extension.

---

## Features
- Builtin commands:
  - `help` — list available builtins
  - `cd <dir>` — change current working directory
  - `banner` — print the DShell ASCII banner
  - `!!` — replay the last executed job
  - `exit` — exit the shell
- Pipelines: `cmd1 | cmd2 | cmd3`
- Background jobs: `cmd &`
- I/O redirection:
  - input redirection: `< file`
  - output redirection: `> file`
- Modular architecture:
  - lexical/tokenization layer
  - command parsing layer
  - job construction and execution layer
- Checked for memory leaks with Valgrind

---

## Building
DShell is built using an existing `Makefile`.

Available targets:
- `make build` — build the shell
- `make run` — build and run DShell
- `make bdebug` — build with debug flags
- `make valgrind` — run DShell under Valgrind

---

## Running
Start the shell with:

    make run


Notes:
- `!!` replays the last job by cloning it and re-executing it.
- Background jobs are periodically reaped and removed when finished.

---

## Project Structure
- `main.c` — program entry point
- `dshell.h`, `shell.c` — shell lifecycle and job list management
- `line_tokenizer.c` — tokenization of input lines
- `parser.c` / `parser.h` — command parsing and redirection handling
- `commands.c` / `commands.h` — command abstraction and execution
- `jobs.c` / `jobs.h` — job construction, pipelines, and process group handling
- `builtins.c` / `builtins.h` — builtin command implementations
- `args.c` / `args.h` — helpers for managing `char **` argument arrays
- `sf_wraps.c` / `sf_wraps.h` — safe wrappers for memory allocation
- `read_write.c` / `read_write.h` — prompt, input, and error output
- `ui.c` / `ui.h` — banner and UI helpers

---

## Architecture Overview
1. Input  
   `read_line()` reads a full input line from stdin.

2. Lexing  
   `tokenize_line()` splits the line into tokens based on whitespace.

3. Job construction  
   `build_job()`:
   - detects background execution (`&`)
   - splits pipelines on `|`
   - builds `Command` objects and assigns parsers

4. Execution  
   `launch_job()`:
   - allocates pipes
   - forks child processes or executes parent-only builtins
   - assigns a process group ID
   - waits for foreground jobs

5. History  
   Successful jobs are copied into `lastJob`; `!!` clones and re-runs that job.

---

## Known Issues
- Open file descriptor leaks  
  Some file descriptors are not closed when commands are freed.
  This is a known issue and causes pipelines to fail when replayed via history (`!!`).

- History + pipes  
  Due to FD leakage, pipelines do not reliably work when executed from history.

- Incomplete features:
  - no globbing / filename expansion
  - no quote handling (`"..."`, `'...'`)
  - no tab auto-completion

---

## Valgrind
The project has been tested with Valgrind.
Memory leaks are mostly addressed, but file descriptor leaks remain.

To run Valgrind:

    make valgrind

---

## TODO / Future Work
- Close all command-level file descriptors when freeing commands
- Fix pipeline replay via history
- Add globbing support
- Add quote parsing
- Add tab auto-completion
- Improve job list resizing and error handling
- Expand redirection support

---

## Design Notes
- Builtins that must affect shell state (for example `cd`, `exit`, `!!`) are marked as `parent_only` and run in the parent process.
- External commands are executed using `fork()` and `execvp()`.
- Pipelines are implemented using POSIX pipes with explicit file descriptor management.
- Process groups are used to manage foreground and background jobs.
