# Rusthon++

A statically and strongly typed, general-purpose scripting language — designed and implemented from the ground up in **C++23**, with no external compiler tooling (no LLVM, no parser generators). Every stage of the source-to-execution pipeline is handwritten: **lexer → parser → interpreter**.

**Example code:**

```
# predicate for filtering
def bool is_adult(copy int age) do
    ret age >= 18
done

# function modifying a reference and returning a subset
def int[] process_users(mut int valid_count, int[] ages) do
    int[] adults = ages ? is_adult      # filter collection by predicate
    valid_count = $adults               # collection cardinality
    ret adults
done

var int adult_count = 0
int[] raw_ages = [12, 19, 25, 15, 30]

int[] processed = process_users(adult_count, raw_ages)
# processed   == [19, 25, 30]
# adult_count == 3
```

## Highlights

- **Full pipeline, no compiler tooling** — lexical analysis, parsing, and execution implemented entirely from scratch: no LLVM, no lexer/parser generators. Third-party code is limited to utilities (`magic_enum` for error/AST printing, `argparse` for the CLI) and GoogleTest for the test suite.
- **Lazy, streaming lexer** — tokens are produced on demand, one at a time, at the parser's request. Tokenization is fully decoupled from parsing and memory overhead stays minimal regardless of input size.
- **Eager LL(1) recursive descent parser** — builds a well-formed AST in a single pass with one token of lookahead, enforcing the language's static type rules already at parse time.
- **Single-pass tree-walking interpreter** — evaluates the AST directly using the **Visitor pattern**, with full support for the type system, scoping, shadowing, and reference semantics.
- **456 test cases** covering all three stages independently (lexer, parser, interpreter) via GoogleTest.
- **Rich diagnostics** — fail-fast error reporting with precise line/column locations across all stages; runtime errors additionally include a call traceback.

## Language at a Glance

| Design axis | Choice |
| :--- | :--- |
| Typing | Static, strong, no implicit conversions (`cast_to` for explicit casts) |
| Mutability | **Immutable by default**; `var` opts into mutability |
| Argument passing | **By reference by default** (const ref); `mut` for mutable refs, `copy` for pass-by-value |
| Scoping | Lexical block scoping with shadowing; scope stack makes a GC unnecessary |
| Functions | First-class definitions, overloading by arity and types, recursion (depth-limited) |
| Collections | Homogeneous arrays with built-in functional operators |

### Notable features

**Functional collection operators** — filtering, mapping, cardinality, reversal, concatenation, and difference are first-class operators:

```
int[] nums = [1, 2, 3, 4, 5]

int[] big      = nums ? isBig          # filter by predicate
int[] doubled  = nums -> makeDouble    # map with a function
int  count     = $nums                 # cardinality
int[] reversed = @nums                 # reversal
```

**Explicit ownership at call sites** — the parameter modifier tells the reader exactly what a function may do with an argument:

```
def int[] process(mut int counter, int[] data, copy int limit) do
    # counter: mutable reference — caller sees changes
    # data:    const reference   — read-only, no copy
    # limit:   pass-by-value     — local, independent copy
    ...
done
```

**Strong typing enforced end to end** — conditions require `bool` (no truthiness), operators require matching operand types, and all conversions are explicit:

```
int b = (3.20 + 3.90) cast_to int   # b == 7 — explicit, or it doesn't compile
```

The full specification — EBNF grammar, token definitions, type conversion rules, operator precedence table, and the complete error taxonomy — lives in [`docs/`](docs/).

## Architecture

```
source code ──▶ Lexer ──▶ Parser ──▶ Interpreter ──▶ output
              (lazy,     (LL(1)      (tree-walking,
               streaming  recursive   Visitor pattern)
               tokens)    descent,
                          builds AST)
```

The pipeline is pull-based: the interpreter requests an AST from the parser, and the parser requests tokens from the lexer one at a time. Nothing is materialized before it is needed.

- **Lexer** — consumes a character stream and emits tokens on demand. Handles numeric literals with digit separators (`1_000_000`), escape sequences, and produces a synthetic `EOF` token past end of input.
- **Parser** — recursive descent over the token stream with a fixed grammar (LL(1)), producing an AST and rejecting malformed programs with targeted syntax exceptions.
- **Interpreter** — walks the AST via the Visitor pattern. Scoping is implemented as a two-dimensional variable table (variables × scopes): entering a block pushes a scope, exiting pops it — deterministic cleanup with no garbage collector.

Each stage is a separate module with its own public interface (`include/Lexer`, `include/Parser`, `include/Interpreter`) and its own test suite.

## Project Structure

```
.
├── app/          # CLI entry point
├── docs/         # full language specification (grammar, semantics, error taxonomy)
├── include/
│   ├── Drivers/
│   ├── Exceptions/
│   ├── Interpreter/
│   ├── Lexer/
│   └── Parser/
├── src/
│   ├── Drivers/
│   ├── Interpreter/
│   ├── Lexer/
│   └── Parser/
└── test/
    ├── AuxTest/
    ├── InterpreterTest/
    ├── LexerTest/
    └── ParserTest/
```

## Building

Requirements: Clang with C++23 support, CMake ≥ 3.x, Ninja, LLD.

```sh
cmake --preset clang-ninja-lld
cmake --build build
```

## Usage

Run a script:

```sh
./build/bin/Main script.txt
```

A program can terminate itself with the built-in `exit()`. The lexer additionally supports an interactive mode for inspecting the token stream live.

## Testing

456 test cases across all three stages, built on GoogleTest (fetched automatically via `FetchContent`):

```sh
cd build && ctest
```

## Error Reporting

Fail-fast at every stage, with precise source locations:

```
Error: Lexer Exception at Line: 1, Column: 1
Unterminated string literal "a...". Expected closing quote before the end of the line.
                             ~~~^
```

Runtime errors include a traceback. The full error taxonomy (11 lexical, 11 syntactic, 14 semantic/runtime exception types) is documented in [`docs/`](docs/).

## Toolchain

C++23 · Clang · CMake (presets) · Ninja · LLD · GDB · GoogleTest · magic_enum · argparse