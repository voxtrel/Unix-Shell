# Unix-Shell
This is a Unix Shell called Rush written in C. It implements built-in functions such as exit, cd, path, and runs external programs using fork()/execv(). Features include output redirection and running multiple commands in parallel.

> Note: This program targets POSIX systems (Linux, macOS). On Windows, use WSL/MSYS2. macOS is Unix (Darwin), not Linux, but it supports the needed POSIX APIs.

---

## Features
- Built‑in functions: `exit`, `cd <dir>`, `path <dir1> <dir2> ...`
- Execute external commands 
- Output redirection 
- Parallel execution

## Requirements
- POSIX environment (Linux, macOS) with a C toolchain
- Uses `getline`, `strsep`, `fork`, `execv`, `waitpid`, `access`, `open`
- Not intended for MSVC/Windows CRT directly (use WSL/MSYS2 if on Windows)

## Build

Using the provided `Makefile`:

```bash
make          # builds the 'rush' binary
make clean    # removes the binary
```

Or directly with gcc/clang:

```bash
gcc -std=gnu11 -Wall -Wextra -O2 rush.c -o rush
# or
clang -Wall -Wextra -O2 rush.c -o rush
```

## Run

```bash
./rush
# You should see:
# rush>
```

## Usage

### Built‑in Functions
- `exit` — terminate the shell.
- `cd <directory>` — change the current working directory.
- `path <dir1> <dir2> ...` — replace the shell's search path. The default is `/bin:/usr/bin`.

### External commands
Any non built‑in is searched in the configured `path` (left‑to‑right) and executed with `execv()` if found.

### Output redirection
Syntax: `command args ... > filename`

Rules enforced by the shell:
- Exactly one `>` per command.
- A single filename must follow `>`.
- No extra arguments after the filename.

Example:
```bash
echo hello > out.txt
cat out.txt
```

### Parallel commands
Use `&` to separate commands to run at the same time:
```bash
sleep 1 & echo done & date
```

The shell waits for all child processes to finish before showing the next prompt.

## Example session
```
rush> echo hello > hello.txt
rush> cat hello.txt
hello
rush> sleep 1 & echo fast & ls
fast
rush.c  Makefile  hello.txt  rush
rush> cd ..
rush> exit
```
