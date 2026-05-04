# LL1

```EBNF
Program            ::== StatementList
StatementList      ::== { Statement NEWLINE }
Statement          ::== FunctionDef // "def"
                      | VarDecl // "var"
                      | ConstDecl // Type
                      | IfStmt // "if"
                      | WhileStmt // "while"
                      | LoopControl // "break"/"continue"
                      | ReturnStmt // "ret"
                      | ExpressionStmt // else

FunctionDef        ::== "def" ReturnType IDENTIFIER "(" [ ParamList ] ")" Block
ReturnType         ::== Type | "void"
ParamList          ::== Param { "," Param }
Param              ::== [ ParamModifier ] Type IDENTIFIER
ParamModifier      ::== "copy" | "mut"
Block              ::== "do" StatementList "done"

VarDecl            ::== "var" Type IDENTIFIER "=" Expression
ConstDecl          ::== Type IDENTIFIER "=" Expression
ExceptionDecl      ::== "exception" IDENTIFIER

IfStmt             ::== "if" "(" Expression ")" Block
                        { "elseif" "(" Expression ")" Block }
                        [ "else" Block ]

WhileStmt          ::== "while" "(" Expression ")" Block
LoopControl        ::== "break" | "continue"
ReturnStmt         ::== "ret" [ Expression ]

Type               ::== BaseType | ArrayType
BaseType           ::== "int" | "float" | "char" | "string" | "bool" 
ArrayType          ::== "[" Type "]"

ExpressionStmt     ::== Expression
Expression         ::== LogicalOrExpr [ AssignmentOp Expression ]
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
ArrayLiteral       ::== "[" [ ArgumentList ] "]"
Literal            ::== INT_LITERAL 
                      | FLOAT_LITERAL 
                      | STRING_LITERAL 
                      | CHAR_LITERAL 
                      | BOOL_LITERAL
```
