# LL1

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
