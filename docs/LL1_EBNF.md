# LL1

```EBNF
Program            ::== { NEWLINE } StatementList
StatementList      ::== { Statement ( NEWLINE {NEWLINE} | EOF )  }
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
Block              ::== "do" NEWLINE StatementList "done" NEWLINE

VarDecl            ::== "var" Type IDENTIFIER "=" Expression
ConstDecl          ::== Type IDENTIFIER "=" Expression

IfStmt             ::== "if" ParenthExprAndBlock
                        { "elseif" ParenthExprAndBlock }
                        [ "else" Block ]
                        NEWLINE
ParenthExprAndBlock::== "(" Expression ")" Block

WhileStmt          ::== "while" ParenthExprAndBlock NEWLINE
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
ArrayLiteral       ::== "[" [ ArgumentList ] "]"
Literal            ::== INT_LITERAL 
                      | FLOAT_LITERAL 
                      | STRING_LITERAL 
                      | CHAR_LITERAL 
                      | BOOL_LITERAL
```
