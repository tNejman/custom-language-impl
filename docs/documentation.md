# Rusthon++ Language Specification

## 1. General Information

* Author: Tomasz Nejman
* Course: Compilation Techniques (TKOM)
* Language name: Rusthon++
* Topic: "static/strong typing / immutable by default / pass by reference"

## 2. Requirements Analysis

### 2.1. Functional Requirements

* **Typing and variables:** The language is statically and strongly typed. It allows creating variables, assigning values to them, and reading them. Variable semantics assume they are constant by default (a value is assigned once) with the option to explicitly declare them as mutable. The language has defined variable visibility scopes with a shadowing mechanism inside blocks.
* **Numeric types and operations:** The language supports basic numeric data types (integer `int` and floating-point `float`). It provides support for mathematical operations with different execution priorities and allows the use of parentheses to group expressions. The lexical analyzer supports separator characters in numbers (e.g. `1_000_000`).
* **Boolean type and operations:** The language supports a boolean type (`bool`). It provides support for logical operations with different execution priorities and allows the use of parentheses to group expressions.
* **Character and text types:** The language supports a character type (`char`) and a text type (`string`). Operations on character sequences are defined, including at minimum concatenation. Text strings may contain arbitrary characters, including string delimiters (e.g. a quotation mark or apostrophe preceded by an escape character), newline characters, and tabs.
* **Comments:** The language supports single-line comments.
* **Control flow:** The language supports conditional statements (`if`, `elseif`, `else`) and loop statements (`while`, `break`, `continue`).
* **Functions:** The language allows defining and calling user-defined functions, which have their own local variables. Functions can be called recursively.
* **Argument passing:** The language semantics define how arguments are passed to functions. By default, values are references, while an explicit option to pass variables by value has been implemented using the dedicated `copy` keyword.
* **Arrays**: The language supports homogeneous collections (arrays) with an assigned set of built-in operators (including cardinality, mapping, filtering).

### 2.2. Non-functional Requirements

* Code analysis is divided into modules:
  * lexer
  * parser
  * interpreter
* Each module is written in C++23.
* The interpreter is a console application (CLI) and allows loading code from a file or entering it in interactive mode.
* The roles of the modules are as follows:
  * lexer - reads live text / accepts a stream of characters (code) and produces a stream of tokens
  * parser - receives the token representation from the lexer and builds an AST tree
  * interpreter - traverses the tree, visiting nodes and executing the program.

### 3. Formal Syntax (Grammar)

### 3.1. Terminal Symbols (Lexer)

```regex
COMMENT        ::= r"#.*"

INT_LITERAL    ::= r"[0-9]+(_[0-9]+)*"
FLOAT_LITERAL  ::= r"[0-9]+(_[0-9]+)*\.[0-9]+(_[0-9]+)*"
STRING_LITERAL ::= r'"[^"\\]*(\\.[^"\\]*)*"'
CHAR_LITERAL   ::= r"'[^'\\]'|'\\[nrt\'\"\\]'"
BOOL_LITERAL   ::= r"\b(true|false)\b"

TYPE_INT       ::= r"\bint\b"
TYPE_FLOAT     ::= r"\bfloat\b"
TYPE_CHAR      ::= r"\bchar\b"
TYPE_STRING    ::= r"\bstring\b"
TYPE_BOOL      ::= r"\bbool\b"
TYPE_VOID      ::= r"\bvoid\b"

KW_IF          ::= r"\bif\b"
KW_ELSEIF      ::= r"\belseif\b"
KW_ELSE        ::= r"\belse\b"
KW_WHILE       ::= r"\bwhile\b"
KW_DO          ::= r"\bdo\b"
KW_DONE        ::= r"\bdone\b"
KW_BREAK       ::= r"\bbreak\b"
KW_CONTINUE    ::= r"\bcontinue\b"
KW_RET         ::= r"\bret\b"

OP_AND         ::= r"\band\b"
OP_OR          ::= r"\bor\b"
OP_NOT         ::= r"\bnot\b"

KW_DEF         ::= r"\bdef\b"
KW_VAR         ::= r"\bvar\b"
KW_COPY        ::= r"\bcopy\b"
KW_CAST        ::= r"\bcast_to\b"
KW_MUT         ::= r"\bmut\b"

OP_MAP         ::= r"->"
OP_EQ          ::= r"=="
OP_NEQ         ::= r"!="
OP_GEQ         ::= r">="
OP_LEQ         ::= r"<="

OP_ADD_ASSIGN  ::= r"\+="
OP_SUB_ASSIGN  ::= r"-="
OP_MUL_ASSIGN  ::= r"\*="
OP_DIV_ASSIGN  ::= r"/="

OP_CONCAT      ::= r"\+\+"
OP_DIFF        ::= r"--"

OP_ASSIGN      ::= r"="
OP_PLUS        ::= r"\+"
OP_MINUS       ::= r"-"
OP_MUL         ::= r"\*"
OP_DIV         ::= r"/"
OP_MOD         ::= r"%"
OP_LEN         ::= r"\$"
OP_FILTER      ::= r"\?"
OP_GT          ::= r">"
OP_LT          ::= r"<"
OP_REV         ::= r"@"

COLON          ::= r":"
COMMA          ::= r","
LPAREN         ::= r"\("
RPAREN         ::= r"\)"
LBRACKET       ::= r"\["
RBRACKET       ::= r"\]"

IDENTIFIER     ::= r"[a-zA-Z][a-zA-Z0-9_]*"

NEWLINE        ::= r"\r?\n"
WS             ::= r"[ \t]+"
```

The `COMMENT` and `WS` tokens are passed to the parser, but it discards them.
When the input text ends, the Lexer, when asked for the next token, returns an artificial EOF token indefinitely, with a position 1 column past the last line of the program.

### 3.2. EBNF Grammar (Parser)

```EBNF
Program            ::== StmtOrFunDefList EOF
StmtOrFunDefList   ::== { NEWLINE } { ( Statement | FunctionDef ) NEWLINE { NEWLINE } }

Statement          ::== FunctionDef
                      | VarDecl
                      | ConstDecl
                      | IfStmt
                      | WhileStmt
                      | LoopControl
                      | ReturnStmt
                      | ExpressionStmt

FunctionDef        ::== "def" ReturnType IDENTIFIER "(" [ ParamList ] ")" Block
ReturnType         ::== Type | "void"
ParamList          ::== Param { "," Param }
Param              ::== [ ParamModifier ] Type IDENTIFIER
ParamModifier      ::== "copy" | "mut"
Block              ::== "do" NEWLINE StatementList "done"
StatementList      ::== { NEWLINE } { Statement NEWLINE { NEWLINE } }

VarDecl            ::== "var" Type IDENTIFIER "=" Expression
ConstDecl          ::== Type IDENTIFIER "=" Expression

IfStmt             ::== "if" ParenthExprAndBlock
                        { "elseif" ParenthExprAndBlock }
                        [ "else" Block ]
ParenthExprAndBlock::== "(" Expression ")" Block

WhileStmt          ::== "while" ParenthExprAndBlock
LoopControl        ::== "break" | "continue"
ReturnStmt         ::== "ret" [ Expression ]

Type               ::== BaseType { "[" "]" }
BaseType           ::== "int" | "float" | "char" | "string" | "bool" 

ExpressionStmt     ::== Expression
Expression         ::== LogicalOrExpr [ AssignmentOp LogicalOrExpr ]
AssignmentOp       ::== "=" | "+=" | "-=" | "*=" | "/="
LogicalOrExpr      ::== LogicalAndExpr { "or" LogicalAndExpr }
LogicalAndExpr     ::== EqualityExpr { "and" EqualityExpr }
EqualityExpr       ::== RelationalExpr { ( "==" | "!=" ) RelationalExpr }
RelationalExpr     ::== AdditiveExpr { ( "<" | "<=" | ">" | ">=" ) AdditiveExpr }
AdditiveExpr       ::== MultiplicativeExpr { ( "+" | "-" | "++" | "--" ) MultiplicativeExpr }
MultiplicativeExpr ::== UnaryExpr { ( "*" | "/" | "%" ) UnaryExpr }
UnaryExpr          ::== ( "-" | "not" | "@" | "$" ) UnaryExpr 
                      | CastExpr
CastExpr           ::== AccessExpr { "cast_to" Type }
AccessExpr         ::== PrimaryExpr { "[" Expression "]" | "->" IDENTIFIER | "?" IDENTIFIER }
PrimaryExpr        ::== IDENTIFIER [ "(" [ ArgumentList ] ")" ]
                      | Literal
                      | "(" Expression ")"
                      | ArrayLiteral

ArgumentList       ::== Expression { "," Expression }
ArrayLiteral       ::== "[" ArgumentList "]"
Literal            ::== INT_LITERAL 
                      | FLOAT_LITERAL 
                      | STRING_LITERAL 
                      | CHAR_LITERAL 
                      | BOOL_LITERAL
```

## 4. Semantics and Language Constructs

To illustrate the intended functionality before the detailed description, below is a code usage example:

```Rusthon++
# predicate for filtering
def bool is_adult(copy int age) do
    ret age >= 18
done

# function modifying a reference and returning a subset
def int[] process_users(mut int valid_count, int[] ages) do
    int[] adults = ages ? is_adult      # filtering the collection against a predicate
    valid_count = $adults               # cardinality of the collection
    ret adults
done

var int adult_count = 0
int[] raw_ages = [12, 19, 25, 15, 30]

# transforming a mutable variable through a reference
int[] processed = process_users(adult_count, raw_ages)
# Values after execution:
#   processed == [19, 25, 30]
#   adult_count == 3
```

### 4.1. Type System and Variables

#### 4.1.1. Declarations and Passing

Variables are initialized as constants by default. To declare a mutable variable, the `var` keyword is used. The variable's type is static and is specified explicitly before the variable/constant identifier. The type of a variable/constant is immutable. A variable declaration cannot occur without a value assignment.

The assignment operator `=` always performs a deep copy of the value.

Examples of declaration and initialization:

```Rusthon++
int max_users = 10     # constant by default
var int counter = 0    # mutable variable
```

A correct example, changing the value within one type:

```Rusthon++
var string message = "hello"
message = "world"
```

An incorrect example of changing a variable (constant):

```Rusthon++
var int age = 20
age = "abc"      # error - type mismatch

int id = 123
id = 456         # error - attempt to modify a constant
```

Example of copying via the `=` operator:

```Rusthon++
int original_counter = 5
var int counter_copy = original_counter  # counter_copy is a new independent variable
counter_copy = 10                        # counter_copy is a variable, even though original_counter was a constant
```

References in Rusthon++ exist only when passing arguments to function calls. Ways of passing arguments:

1. Constant reference (default): no modifier before the parameter type (e.g. `int[] nums`).
2. Mutable reference: the `mut` keyword before the parameter type (e.g. `mut int valid_count`). For the call to be valid, the argument passed to the function must be mutable.
3. Pass by value: the `copy` keyword (e.g. `copy int age`).

#### 4.1.2. Basic Data Types

* The language provides a signed integer type `int`. Legal `int` values are in the range:
  * from `-2147483648`
  * to `2147483647`.
* The language supports the `_` separator character for easier entry of large literals, e.g. `int x = 1_000_000`
* The language provides a signed single-precision floating-point type `float`. Value bounds for the `float` type:
  * from `-2147483648.0`
  * to `2147483647.0`.
* Entering a literal with a value greater than MAX or less than MIN results in an error.
* The language provides a boolean type `bool` with possible values `true` or `false`.

### 4.1.3. Character Types, Text Types, and Comments

The character type `char` is written in single quotes, while character strings `string` are written in double quotes. The language supports escape characters using the backslash `\`, allowing the entry of a newline `\n`, tab `\t`, text delimiters `\"`, `\'`, and the backslash `\\`. The compound type `string` is equivalent to the type `char[]` (an array of `char`), and therefore any collection operators can be used on it.

```Rusthon++
char letter = 'A'
char quote = '\"'
string message = "He said: \"Hello World!\"\n"
```

```Rusthon
string word1 = "Hello,"
string word2 = "World"
string word3 = "!\n"

string result = word1 ++ @word2 ++ word3  # result == "Hello, dlroW!\n"
```

Single-line comments start with the `#` character. Everything to the right of this character on a given line is ignored.

```Rusthon++
int a = 5 # This is a valid comment on a line with code
```

### 4.1.4. Type Conversion (Casting)

The language is strongly typed and does not perform implicit conversions. To perform a conversion, the `cast_to` keyword operator must be used, followed by the type being cast to. It is used to convert between compatible types (e.g. `int` to `float`) and values. The operation creates a new, mutable value and ensures its mathematical/logical meaning is preserved. If the conversion cannot be performed at runtime, a runtime exception is thrown. The `cast_to` operator has the highest priority (excluding parentheses) and binds more tightly than arithmetic operators.

Example of a correct and incorrect conversion:

```Rusthon++
float a = 3 cast_to float          # a == 3.0
int b = (3.20 + 3.90) cast_to int  # b == 7

float c = "abc" cast_to float  # error - no conversion from type 'string' to 'float' for the value "abc"
```

#### The conversion rules for basic types are presented in the table below

(`cast_to` naturally works when casting from type `T` to `T`, simply copying the value)

| From\To | `int` | `float` | `char` | `bool` |
| ---- | ----- | ------- | ------ | ------ |
| `int` | N/A | `.0` is appended | Unicode code, e.g. `65`$\rightarrow$`'A'` | `false` for `0`, `true` for the rest |
| `float` | truncation of the decimal part, towards zero | N/A | not allowed | `false` for `0.0`, `true` for the rest |
| `char` | Unicode values, e.g. `'A'`$\rightarrow$`65` | not allowed | N/A | not allowed |
| `bool` | `1` for `true` or `0` for `false` | `1.0` for `true` or `0.0` for `false` | not allowed | N/A |

#### Arrays are also subject to conversion

```Rusthon++
[1, 2, 3] cast_to float[] # returns [1.0, 2.0, 3.0]
```

### 4.2. Operators and Expressions

#### 4.2.1 Operator Precedence

Precedence goes from highest to lowest. Within each precedence level, the operator on the left takes priority.

1. `()` - parentheses, left-associative
2. `[]`, `->`, `?` - access and transformations, left-associative
3. `cast_to` - type casting, left-associative
4. `-`, `not`, `@`, `$` - unary operators, right-associative
5. `*`, `/`, `%` - multiplicative, arithmetic, left-associative
6. `+`, `-`, `++`, `--` - additive, arithmetic and collections, left-associative
7. `<`, `<=`, `>`, `>=` - relational (inequalities), left-associative
8. `==`, `!=` - relational (equalities), left-associative
9. `and` - logical AND, left-associative
10. `or` - logical OR, left-associative
11. `=`, `+=`, `-=`, `*=`, `/=`, `%=` - assignment, right-associative

#### 4.2.2. Assignment Operators

Every assignment operator requires operands of the same type.
Every arithmetic assignment operator (all of them here except `=`) requires operands of type `int` or `float`.

1. Standard assignment `=`
2. Addition assignment `+=`
3. Subtraction assignment `-=`
4. Multiplication assignment `*=`
5. Division assignment `/=`
6. Modulo assignment `%=`

#### 4.2.3. Arithmetic Operations

Every operator requires two operands. Both must be of the same type and be `int` or `float`, with the exception of modulo, which accepts only `int`.

1. Addition `+`
2. Subtraction `-`
3. Multiplication `*`
4. Division `/`
5. Modulo `%`

#### 4.2.4. Logical and Relational Operations

1. `not` - one operand of boolean expression type, right-hand side
2. `and` - two operands of boolean expression type
3. `or` - two operands of boolean expression type

```Rusthon++
if (y != 0 and x / y > 5) do
    # code block
done
```

1. Greater than `>`
2. Less than `<`
3. Greater than or equal `>=`
4. Less than or equal `<=`
5. Equal `==`
6. Not equal `!=`

Every operator requires two operands. Both must be of the same type.
The inequality operators require the type `int`, `float`, or `char`. The equality operators `==` and `!=` work for any type and compare by value.

#### 4.2.5. Special Operators

1. Parentheses `()`

For artificially raising the priority of an operation or for function calls.

#### 4.2.6. Behavior in Unusual Situations

1. Types `int` and `float`

If the upper maximum value or the lower minimum value is exceeded, the result of the operation is clamped, e.g.

```Rusthon++
2147483647 + 1 # returns 2147483647
```

### 4.3. Complex Data Structures

#### 4.3.1. Collections and Their Operators

The language supports homogeneous collections (arrays). Collections can be created on the fly using square brackets `[]`. The assignment operator passes collections by deep copy by default. Positions in collections are indexed from `0`.

Defined operators for collections:

1. `[]` - element access
2. `++` - concatenation of two collections
3. `--` - difference of two collections
4. `$` - cardinality of the collection (number of elements)
5. `@` - reversal of the collection (unary operator)
6. `?` - filtering against a predicate (expects an identifier of a function with signature `bool foo(T)`)
7. `->` - mapping with a function (expects an identifier of a function with signature `T1 foo(T2)`, where `T2` is the type in the collection)

Examples of operator usage:

```Rusthon++
int[] nums1 = [1, 2, 3]
int[] nums2 = [3, 4, 5]

int[] combined = nums1 ++ nums2    # [1, 2, 3, 3, 4, 5]
int[] sub = nums1 -- nums2         # [1, 2]
int count = $nums1                 # 3
int[] reversed = @nums             # [3, 2, 1]
```

```Rusthon++
# predicate for filtering
def bool isBig(copy int x) do
    ret x > 3
done

int[] big_nums = nums2 ? isBig     # [4, 5]
```

```Rusthon++
# function for mapping
def int makeDouble(copy int x) do
    ret x * 2
done

int[] nums_double = nums1 -> makeDouble   # [2, 4, 6]
```

\* the mapping operator `->` allows mapping with a function from any type `T2` to any other type `T1`, e.g.

```Rusthon++
def bool isSmall(copy int x) do
    ret x < 0
done

int[] nums = [-2, -1, 0, 1, 2]
bool[] is_small_arr = nums -> isSmall    # is_small_arr == [true, true, false, false, false]
```

### 4.4. Control Flow

#### 4.4.1. Control Statements

The language supports conditional statements (keywords: `if`, `elseif`, `else`) and loops (`while`). Their blocks are opened with the `do` keyword; indentation is arbitrary. The end of a block is marked with the `done` keyword. One statement per line is allowed. During a `while` loop, the remaining operations in a given iteration can be skipped using the `continue` keyword. The loop can also be interrupted with the `break` keyword. `if`, `elseif`, and `while` require specifically the `bool` type (strong typing).

Example of conditional statements:

```Rusthon++
var int x = 3
if (x < 1) do
    x = x + 1
done elseif (x < 5) do
    x = x + 2
done else do
    x = x + 3
done
```

Loop example:

```Rusthon++
var int x = 5
while (x < 10) do
    x = x + 1
done
```

Example of skipping and breaking a loop:

```Rusthon++
var int x = 5
while (x < 100) do
    x = x + 1
    if (x % 17 == 0) do
        continue
    done
    if (x == 19) do
        break
    done
done
```

#### 4.4.2. Functions, Scope, and Variable Shadowing

Functions are defined with the `def` keyword, followed by the return type, identifier, and argument list in parentheses. The `ret` keyword returns a value from a function. The absence of a return value is denoted by `void`, and the `ret` keyword is then not required. The function body also starts with the `do` keyword and ends with `done`. If a function declares a return type that is not `void`, and the interpreter reaches the end of the function without finding the `ret` keyword, an error is thrown.

Functions can be overloaded. The interpreter distinguishes them based on the number and types of arguments. The language's built-in functions are subject to the same rules and can be shadowed or overloaded by the user.

Functions always return a copy.

Functions can be called recursively. The recursion limit is 100 calls.

Functions may only appear at the top level of a file.

The language supports variable shadowing. Variables declared inside a block (e.g. a conditional statement) can have the same name as variables in the outer scope. After exiting the block, access to the original variable is restored with its previous state preserved.

Function example:

```Rusthon++
def int add(copy int a, copy int b) do
    ret a + b
done
```

A version of a function taking a mutable reference and returning nothing:

```Rusthon++
def void process(mut int a, mut int b) do
    a += b
done
```

A version of a function taking an immutable reference:

```Rusthon++
def int addTen(int a) do
    ret a + 10
done
```

A version of a function taking two variables passed by value:

```Rusthon++
def int add(copy int a, copy int b) do
    ret a + b
done
```

Overloading functions with the same name:

```Rusthon++
def int add(copy int a, copy int b) do
    ret a + b
done

def int add(copy int a, copy int b, copy int c) do
    ret a + b + c
done
```

Example of the variable shadowing mechanism:

```Rusthon++
var int x = 5
if (x > 5) do
    int x = 10      # shadowing - 'x' in this block is a constant with the value 10
    # x = 11        # error - 'x' in this block is a constant
done
x = 20              # after exiting the block, 'x' is 5 again and is mutable
```

### 4.4. Built-in Input and Output (I/O)

The language provides two functions, `read` and `write`, with signatures `string read()` and `void write(T)`. The `read` function takes no arguments and reads user input typed into the terminal; it always returns the `string` type. The `write` function can take one argument of any type — since the user cannot define their own types, the behavior is always known — and returns nothing. The `write` function does not automatically move the cursor to the next line; `'\n'` must be used manually.

Usage examples:

```Rusthon++
string name = read()               # we enter "Jan"
write("Hello, " ++ name ++ '\n')   # >> "Hello, Jan"
```

```Rusthon++
int[] nums = [1,2,3,4]
var int i = 0
while (i < 5) do
    write(nums[i])
    write(' ')
    i += 1
done

# >> 1 2 3 4
```

```Rusthon++
string[] names = ["Anna", "Bob", "Charles"]
write(names)     # [ "Anna", "Bob", "Charles" ]
```

### 5. Error Reporting and Diagnostics

A "fail-fast" strategy is used. This means that upon encountering an error at any stage, the program terminates with an error.

### 5.1. Types of Reported Errors

#### 5.1.1. Lexical Errors

Detected by the lexer during tokenization.

| Thrown exception | Cause |
| :--- | :--- |
| FloatLiteralOutOfBoundsException | Exceeding the value bounds of the floating-point type |
| IntLiteralOutOfBoundsException | Exceeding the value bounds of the integer type |
| InvalidCharLiteralException | Invalid character literal format (e.g. empty quotes `''` or too many characters) |
| InvalidCommentStyleException | Disallowed comment style /**/ |
| MalformedNumericLiteralException | Malformed / incorrectly formatted numeric literal (e.g. `0xG1` or `1.2.3`) |
| TooLongIdentifierException | Exceeding the maximum allowed identifier (name) length |
| TooLongStringLiteralException | Exceeding the maximum allowed string literal length |
| UnknownEscapedCharacterException | Use of an unknown escape sequence (e.g. `\z` inside text) |
| UnknownSymbolException | Encountering an unknown or disallowed symbol in the source code, e.g. `;` |
| UnterminatedCharLiteralException | Unclosed character literal (missing closing apostrophe before the end of the line/file) |
| UnterminatedStringLiteralException | Unclosed string literal (missing closing quote before the end of the line/file) |

#### 5.1.2. Syntax Errors

Detected by the parser while building the AST tree from tokens.

| Thrown exception | Cause |
| :--- | :--- |
| InvalidParameterException | Invalid parameter (e.g. in a function definition or call) |
| InvalidTypeException | Invalid or unrecognized data type |
| MissingBracketException | Missing square bracket `[` / `]` |
| MissingExpressionException | Missing expression (e.g. inside a conditional statement or after an assignment sign) |
| MissingIdentifierException | Missing identifier (e.g. missing variable or function name at a declaration site) |
| MissingKeywordException | Missing language keyword (e.g. `if`, `while`, `return`) |
| MissingNewlineException | Missing newline character (if the grammar requires it to terminate a statement) |
| MissingOperatorException | Missing operator between expressions or operands |
| MissingParenthesisException | Missing round parenthesis `(` or `)` |
| MissingRightOperandException | Missing right operand in a binary expression |
| NotConsumedTokensException | Extra, unconsumed tokens after parsing the main program structure has finished (garbage at the end of the file) |

#### 5.1.3. Semantic and Runtime Errors

Detected by the interpreter during semantic analysis and code execution. Exceptions are thrown from the `InterpreterException` family.

| Thrown exception | Cause |
| :--- | :--- |
| FunctionSignatureMismatchException | Passing an incorrect number or types of arguments to a function (mismatch with its signature) |
| VoidValueException | Attempt to use a value of type `void` in an expression (e.g. assigning the result of a function that returns nothing) |
| NotAllowedTypeException | Use of a data type not allowed in the given operation context |
| InvalidCastException | Invalid or impossible conversion (cast) between data types |
| InvalidOperationException | Attempt to perform a disallowed operation (e.g. using an operator that doesn't match the types at runtime) |
| IndexOutOfBoundsException | Going outside the allowed index range of a collection, array, or string |
| InvalidAccessException | Attempt at illegal or invalid access to a member, variable, or memory area |
| InvalidOverloadException | Invalid or conflicting function overload detected during interpretation |
| InvalidShadowException | Invalid or disallowed shadowing of an identifier in a nested block |
| UnknownIdentifierException | Reference to an undeclared or unknown identifier (variable, function) during program execution |
| EmptyAccumulatorException | Attempt to read a value from an empty accumulator (internal error of the interpreter's register/virtual machine state) |
| BuiltinFunctionInitException | Error during registration or initialization of a built-in function (interpreter API function) |
| InvalidStatementException | Encountering and attempting to execute a logically or structurally invalid statement at runtime |
| StackOverflowException | Overflow of the interpreter's call stack (most often caused by too deep or infinite recursion) |

### 5.2. Example Error Messages

Lexical, syntax, and semantic errors simply report the error location and cause.
Runtime errors also provide a traceback to make debugging easier.

Lexical error:

```Rusthon++
"abc
```

```plaintext
Error: Lexer Exception at Line: 1, Column: 1
Unterminated string literal "a...". Expected closing quote before the end of the line. (Note: Rusthon++ strings cannot span multiple lines)
                             ~~~^
```

Syntax error (missing token):

```Rusthon++
if (true)
    write("hello")
done
```

```plaintext
Error: Parser Exception at Line: 1, Column: 10
Missing Keyword 'KW_DO' when building ''do ... done' block'.
```

Semantic error (attempt to modify a constant):

```Rusthon++
int x = 5
x = 3
```

```plaintext
Error: Interpreter Exception at Line: 2, Column: 1
cannot assign to immutable value
```

## 6. Tools and Lifecycle

### 6.1. Tools (Components)

#### Code Analysis and Execution Process

* The interpreter requests the AST tree from the parser
* The parser reads the token stream until it forms the entire AST tree
* On the parser's request, the lexer reads the character stream and prepares one token at a time
* The interpreter visits the tree and executes it

The exceptions are functions and blocks, which must be loaded in their entirety at once.

#### Variable Shadowing

The variable table is two-dimensional (1-variables, 2-scopes). When a new block is created, e.g. a function body, a new dimension is added (effectively a push onto a stack). When looking up an identifier, the interpreter scans the table from the highest position (the newest scope). When the block ends, the entire dimension is discarded, which means a garbage collector is not needed.

### 6.2. How to Run

```plaintext
interpreter script.txt
```

Ending a session: `exit()`

The built-in `exit()` function is used to terminate the program and can also be used in code.