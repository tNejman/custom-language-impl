# Specyfikacja Języka Rusthon++

## 1. Informacje ogólne

* Autor: Tomasz Nejman
* Przedmiot: Techniki Kompilacji (TKOM)
* Nazwa języka: Rusthon++
* Temat: "typowanie statyczne/silne/domyślnie stałe/przekazywanie przez referencję"

## 2. Analiza wymagań

### 2.1. Wymagania funkcjonalne

* **Typowanie i zmienne:** Język jest statycznie i silnie typowany. Pozwala tworzyć zmienne, przypisywać do nich wartości oraz je odczytywać. Semantyka zmiennych zakłada, że są one domyślnie stałe (przypisanie wartości odbywa się raz) z opcją jawnego określenia ich jako mutowalne. Język posiada zdefiniowane zakresy widoczności zmiennych z mechanizmem przykrywania (shadowing) wewnątrz bloków.
* **Typy liczbowe i operacje:** Język obsługuje podstawowe typy danych liczbowych (całkowitoliczbowe `int` i zmiennoprzecinkowe `float`). Zapewnia obsługę operacji matematycznych o różnym priorytecie wykonania oraz pozwala na użycie nawiasów do grupowania wyrażeń. Analizator leksykalny wspiera znaki separacji w liczbach (np. `1_000_000`).
* **Typ logiczny i operacje:** Język obsługuje typ logiczny (`bool`). Zapewnia obsługę operacji logicznych o różnym priorytecie wykoanania oraz pozawala na użycie nawiasów do grupowania wyrażeń.
* **Typ znakowy i tekstowy:** Język obsługuje typ znakowy (`char`) oraz tekstowy (`string`). Zdefiniowane są operacje na ciągach znaków, w tym minimalnie konkatenacja. Ciągi tekstowe mogą zawierać dowolne znaki, włącznie z wyróżnikami stringa (np. znak cudzysłowu lub apostrofu poprzedzony znakiem ucieczki), znakami nowej linii i tabulacji.
* **Komentarze:** Język obsługuje komentarze jednolinijkowe.
* **Sterowanie przepływem:** Język obsługuje instrukcje warunkowe (`if`, `elseif`, `else`) oraz instrukcje pętli (`while`, `break`, `continue`).
* **Funkcje:** Język daje możliwość definiowania oraz wołania własnych funkcji, które posiadają własne zmienne lokalne. Funkcje mogą być wywoływane rekursywnie.
* **Przekazywanie argumentów:** Semantyka języka definiuje przekazywanie argumentów do funkcji. Domyślnie wartości są referencjami, natomiast wdrożono jawną możliwość przekazywania zmiennych przez wartość za pomocą dedykowanego słowa kluczowego `copy`.
* **Tablice**: Język wspiera homogeniczne kolekcje (tablice) z przypisanym zestawem wbudowanych operatorów (m. in. moc zbioru, mapowanie, filtrowanie).

### 2.2. Wymagania niefunkcjonalne

* Analiza kodu podzielona jest na moduły:
  * lekser
  * parser
  * interpreter
* Każdy z modułów jest napisany w C++23.
* Interpreter jest aplikacją konsolową (CLI) i umożliwia wczytanie kodu z pliku lub wprowadzanie w trybie interaktywnym.
* Role modułów wyglądają następująco:
  * lekser - wczytuje żywy tekst / przyjmuje strumień znaków (kodu) i wyprowadza strumień tokenów
  * parser - otrzymuje reprezentację tokenową z leksera i buduje drzewo AST
  * interpreter - przemierza drzewo odwiedzając węzły i wykonując program.

### 3. Formalna składnia (gramatyka)

### 3.1. Symbole terminalne (Lekser)

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

Tokeny `COMMENT` i `WS` trafiają do parsera, ale on je odrzuca.
Gdy skończy się tekst wejściowy, Lexer zapytany o następny token, zwraca w nieskończoność szutczny token EOF, z pozycją o 1 kolumnę za ostatnią linijką programu.

### 3.2. Gramatyka EBNF (Parser)

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

## 4. Semantyka i konstrukcje językowe

Aby zilustrować zakładaną funkcjonalność przed szczegółowym opisem, poniżej zamieszczam przykład wykorzystania kodu:

```Rusthon++
# predykat do filtrowania
def bool is_adult(copy int age) do
    ret age >= 18
done

# funkcja modyfikująca referencję i zwracająca podzbiór
def int[] process_users(mut int valid_count, int[] ages) do
    int[] adults = ages ? is_adult      # filtrowanie kolekcji względem predykatu
    valid_count = $adults               # moc kolekcji
    ret adults
done

var int adult_count = 0
int[] raw_ages = [12, 19, 25, 15, 30]

# przekształcenie zmiennej mutowalnej przez referencję
int[] processed = process_users(adult_count, raw_ages)
# Wartości po wykonaniu:
#   processed == [19, 25, 30]
#   adult_count == 3
```

### 4.1. System typów i zmienne

#### 4.1.1. Deklaracje i przekazywanie

Zmienne domyślnie inicjowane są jako stałe. Aby zadeklarować zmienną mutowalną, używa się słowa kluczowego `var`. Typ zmiennej jest statyczny i określa się go jawnie przed identyfikatorem zmiennej/stałej. Typ zmiennej/stałej jest niezmienny. Deklaracja zmiennej nie może wystąpić bez przypisania wartości.

Operator przypisania `=` zawsze wykonuje głęboką kopię wartości.

Przykłady deklaracji i inicjalizacji:

```Rusthon++
int max_users = 10     # domyślna stała
var int counter = 0    # zmienna mutowalna
```

Przykład poprawny, zmiana wartości w ramach jednego typu:

```Rusthon++
var string message = "hello"
message = "world"
```

Przykład niepoprawny zmiany zmiennej (stałej):

```Rusthon++
var int age = 20
age = "abc"      # błąd - niezgodność typów

int id = 123
id = 456         # błąd - próba modyfikacji stałej
```

Przykład kopiowania przez operator `=`:

```Rusthon++
int original_counter = 5
var int counter_copy = original_counter  # counter_copy to nowa niezależna zmienna
counter_copy = 10                        # counter_copy jest zmienną, mimo że original_counter było stałą
```

Referencje w języku Rusthon++ istnieją tylko w przypadku przekazwyania argumentów wywołań funkcji. Sposoby przekazywania argumentów:

1. Stała referencja (domyślna): brak modyfikatora przed typem parametru (np. `int[] nums`).
2. Mutowalna referencja: słowo kluczowe `mut` przed typem parametru (np. `mut int valid_count`). Aby wywołanie było poprawne, argument przekazywany do funkcji musi być mutowalny.
3. Przekazywanie przez wartość: słowo kluczowe `copy` (np. `copy int age`).

#### 4.1.2. Podstawowe typy danych

* Język dostarcza typ całkowitoliczbowy ze znakiem `int`. Legalne wartości typu `int` to zakres:
  * od `-2147483648`
  * do `2147483647`.
* Język wspiera znak separacji `_` dla łatwiejszego wprowadzania dużych literałów. np. `int x = 1_000_000`
* Język dostarcza typ zmiennoprzecnikowny pojedynczej precyzji ze znakiem `float`. Granice wartości dla typu `float`:
  * od `-2147483648.0`
  * do `2147483647.0`.
* Wprowadzenie literału o wartości większej od MAX lub mniejszej od MIN kończy się błędem.
* Język dostarcza typ logiczny `bool` o możliwych wartościach `true` lub `false`.

### 4.1.3. Typy znakowe, tekstowe i komentarze

Typ znakowy `char` zapisywany jest w pojedynczych apostrofach, natomiast ciągi znaków `string` w podwójnych cudzysłowach. Język wspiera znaki ucieczki z użyciem ukośnika `\`, pozwalając na wprowadzenie nowej linii `\n`, tabulacji `\t`, wyróżników tekstu `\"`, `\'` oraz ukośnika `\\`. Typ złożony `string` jest jednoznaczny z typem `char[]` (tablica `char`), a zatem możliwe są do użycia na nim dowolne operatory kolekcji.

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

Komentarze jednolinijkowe rozpoczynają się znakiem `#`. Wszystko na prawo od tego znaku w danej linii jest ignorowane.

```Rusthon++
int a = 5 # To jest poprawny komentarz w linii z kodem
```

### 4.1.4. Konwersja typów (casting)

Język charakteryzuje się silnym typowaniem i nie wykonuje niejawnych konwersji. Aby dokonać konwersji należy użyć operatora kluczowego `cast_to`, a po nim typu na który się rzutuje. Służy ono do konwersji pomiędzy kompatybilnymi typami (np. `int` na `float`) i wartościami. Operacja tworzy nową, mutowalną wartość i dba o zachowanie jej sensu matematycznego/logicznego. W przypadku niemożności wykonania konwersji w czasie działania programu, zgłaszany jest wyjątek wykonania. Operator `cast_to` ma najwyższy priorytet (nie licząc nawiasów) i wiąże silniej niż operatory arytmetyczne.

Przykład poprawnej i niepoprawnej konwersji:

```Rusthon++
float a = 3 cast_to float          # a == 3.0
int b = (3.20 + 3.90) cast_to int  # b == 7

float c = "abc" cast_to float  # błąd - brak przejścia z typu 'string' do 'float' dla wartości "abc"
```

#### Zasady konwersji dla podstawowych typów przedstawia poniższa tabela

(`cast_to` oczywiście działają jeżeli rzutowanie jest z typu `T` do `T` po prostu kopiując wartość)

| Z\Do | `int` | `float` | `char` | `bool` |
| ---- | ----- | ------- | ------ | ------ |
| `int` | N/A | dopisuje się `.0` | kod Unicode, np. `65`$\rightarrow$`'A'` | `false` dla `0`, `true` dla reszty |
| `float` | obcięcie części dziesiętnej, w stronę zera | N/A | niedozwolone | `false` dla `0.0`, `true` dla reszty |
| `char` | wartości Unicode, np. `'A'`$\rightarrow$`65` | niedozwolone | N/A | niedozwolone |
| `bool` | `1` dla `true` lub `0` dla `false` | `1.0` dla `true` lub `0.0` dla `false` | niedozwolone | N/A |

#### Konwersji podlegają również tablice

```Rusthon++
[1, 2, 3] cast_to float[] # zwraca [1.0, 2.0, 3.0]
```

### 4.2. Operatory i wyrażenia

#### 4.2.1 Priorytet operatorów

Priorytet jest od największego do najmniejszego. W przypadku każdego priorytetu, wyższość ma operator po lewej.

1. `()` - naziwasy, łączność lewostronna
2. `[]`, `->`, `?` - dostęp i transformacje, łączność lewostronna
3. `cast_to` - rzutowanie typów, łączność lewostronna
4. `-`, `not`, `@`, `$` - operatory unarne, łączność prawostronna
5. `*`, `/`, `%` - multiplikatywne, arytmetyczne, łączność lewostronna
6. `+`, `-`, `++`, `--` - addytywne, arytmetyczne i kolekcje, łączność lewostronna
7. `<`, `<=`, `>`, `>=` - relacyjne (nierówności) łączność lewostronna
8. `==`, `!=` - relacyjne (równości), łączność lewostronna
9. `and` - logiczne AND, łączność lewostronna
10. `or` - logiczne OR, łączność lewostronna
11. `=`, `+=`, `-=`, `*=`, `/=`, `%=` - przypisanie, łączność prawostronna

#### 4.2.2. Operatory przypisania

Każdy operator przypisania wymaga operandów tego samego typu.
Każdy operator przypisania arytmetycznego (każdy tutj poza `=`) wymaga operandów typu `int` lub `float`.

1. Przypisanie standardowe `=`
2. Przypisanie z dodawaniem `+=`
3. Przypisanie z odejmowaniem `-=`
4. Przypisanie z mnożeniem `*=`
5. Przypisanie z dzieleniem `/=`
6. Przypisanie z modulo `%=`

#### 4.2.3. Operacje arytmetyczne

Każdy operator wymaga dwóch operandów. Oba muszą być tego samego typu i być `int` lub `float`, z wyjątkiem modulo, które przyjmuje tylko `int`.

1. Dodawanie `+`
2. Odejmowanie `-`
3. Mnożenie `*`
4. Dzielenie `/`
5. Modulo `%`

#### 4.2.4. Operacje logiczne i relacyjne

1. `not` - jeden operand typu wyrażenie logiczne, prawostronny
2. `and` - dwa operandy typu wyrażenie logiczne
3. `or` - dwa operandy typu wyrażenie logiczne

```Rusthon++
if (y != 0 and x / y > 5) do
    # blok kodu
done
```

1. Większy `>`
2. Mniejszy `<`
3. Większy lub równy `>=`
4. Mniejszy lub równy `<=`
5. Równy `==`
6. Różny `!=`

Każdy operator wymaga dwóch operandów. Oba muszą być tego samego typu.
Operatory nierówności wymagają typu `int` lub `float` lub `char`. Operatory równości `==` oraz `!=` działają dla dowolnego typu i porównują wartość.

#### 4.2.5. Operatory specjalne

1. Nawiasy `()`

Do sztucznego zwiększania priorytetu operacji lub wywołania funkcji.

#### 4.2.6. Zachowanie w nietypowych sytuacjach

1. Typ `int` i `float`

W przypadku przekroczenia górnej wartości maksymalnej lub dolnej wartości minimalnej wynik operacji zostaje przycięty, np.

```Rusthon++
2147483647 + 1 # zwraca 2147483647
```

### 4.3. Złożone struktury danych

#### 4.3.1. Kolekcje i ich operatory

Język wspiera kolekcje homogeniczne (tablice). Kolekcje mogą być tworzone tworzone w locie z użyciem nawiasów kwadratowych `[]`. Operator przypisania domyślnie przekazuje kolekcje przez głęboką kopię. Pozycje w kolekcjach indeksowane są od `0`.

Zdefiniowane operatory dla kolekcji:

1. `[]` - dostęp do elementu
2. `++` - konkatenacja dwóch kolekcji
3. `--` - różnica dwóch kolekcji
4. `$` - moc kolekcji (liczba elementów)
5. `@` - odwrócenie kolekcji (operator unarny)
6. `?` - filtrowanie względem predykatu (oczekuje identyfikatora funkcji o syngaturze `bool foo(T)`)
7. `->` - mapowanie względem funkcji (oczekuje identyfikatora funkcji o sygnaturze `T1 foo(T2)`, gdzie `T2` to typ w kolekcji)

Przykłady użycia operatorów:

```Rusthon++
int[] nums1 = [1, 2, 3]
int[] nums2 = [3, 4, 5]

int[] combined = nums1 ++ nums2    # [1, 2, 3, 3, 4, 5]
int[] sub = nums1 -- nums2         # [1, 2]
int count = $nums1                 # 3
int[] reversed = @nums             # [3, 2, 1]
```

```Rusthon++
# predykat do filtrowania
def bool isBig(copy int x) do
    ret x > 3
done

int[] big_nums = nums2 ? isBig     # [4, 5]
```

```Rusthon++
# funkcja do mapowania
def int makeDouble(copy int x) do
    ret x * 2
done

int[] nums_double = nums1 -> makeDouble   # [2, 4, 6]
```

\* operator mapowania `->` pozwala na mapowanie funkcją z dowlonego typu `T2` na dowolny inny `T1`, np.

```Rusthon++
def bool isSmall(copy int x) do
    ret x < 0
done

int[] nums = [-2, -1, 0, 1, 2]
bool[] is_small_arr = nums -> isSmall    # is_small_arr == [true, true, false, false, false]
```

### 4.4. Przepływ sterowania

#### 4.4.1. Instrukcje sterujące

Język wspiera instrukcje warunkowe (słowa kluczowe: `if`, `elseif`, `else`) oraz pętle (`while`). Ich bloki otwierane otwierane są słowem kluczowym `do`, wcięcie jest dowolne. Koniec bloku wyznaczany jest poprzez słowo kluczowe `done`. Dozwolona jest jedna intrukcja na linijkę. Podczas pętli `while` można ominąć pozostałe operacje w danym przejściu, używając słowa kluczowego `continue`. Można też przerwać pętlę słowem kluczowym `break`. `if`, `elseif` i `while` wymagają konkretnie typu `bool` (silne typowanie).

Przykład instrukcji warunkowych:

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

Przykład pętli:

```Rusthon++
var int x = 5
while (x < 10) do
    x = x + 1
done
```

Przykład pominięcia i przerwania pętli:

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

#### 4.4.2. Funkcje, zakres widoczności i przykrywanie zmiennych

Funkcje definiowane są słowem kluczowym `def`, po którym następuje zwracany typ, identyfikator oraz lista  argumentów w nawiasie. Słowo `ret` zwraca wartość z funkcji. Brak wartości zwracanej oznaczany jest `void`, a słowo kluczowe `ret` nie jest wtedy wymagane. Ciało funkcji również rozpoczyna się słowem kluczowym `do` i kończy `done`. Jeżeli funkcja deklaruje zwracany typ i nie jest nim `void`, a interpreter dojdzie do końca funkcje i nie znajdzie słowa `ret` rzucany jest błąd.

Funkcje mogą być przciążane. Interpreter rozróżnia je na podstawie liczby i typów argumentów. Wbudowane funkcje języka podlegają tym samym zasadom i mogą być przykrywane lub przeciążane przez użytkownika.

Funkcje zawsze zwracają kopię.

Funkcje mogą być wywoływane rekursywnie. Limit rekursji to 100 wywołań.

Funkcje mogą znajdować się tylko na najwyższym poziomie pliku.

Język wspiera przykrywanie zmiennych (shadowing). Zmienne deklarowane wewnątrz bloku (np. instrukcji warunkowej) mogą mieć taką samą nazwę jak zmienne w zakresie zewnętrznym. Po wyjściu z bloku przywracany jest dostęp do oryginalnej zmiennej z zachowaniem jej wcześniejszego stanu.

Przykład funkcji:

```Rusthon++
def int add(copy int a, copy int b) do
    ret a + b
done
```

Wersja funkcji przyjmująca mutowalną referencję i nic nie zwracająca:

```Rusthon++
def void process(mut int a, mut int b) do
    a += b
done
```

Wersja funkcji przjmująca niemutowalną referencję:

```Rusthon++
def int addTen(int a) do
    ret a + 10
done
```

Wersja funkcji przyjmująca dwie zmienne przekazywane przez wartość:

```Rusthon++
def int add(copy int a, copy int b) do
    ret a + b
done
```

Przeciążanie funkcji o tej samej nazwie:

```Rusthon++
def int add(copy int a, copy int b) do
    ret a + b
done

def int add(copy int a, copy int b, copy int c) do
    ret a + b + c
done
```

Przykład mechanizmu przykrywania zmiennych:

```Rusthon++
var int x = 5
if (x > 5) do
    int x = 10      # shadowing - 'x' w tym bloku jest stałą o wartości 10
    # x = 11        # błąd - 'x' w tym bloku jest stałą
done
x = 20              # po wyjściu z bloku 'x' ponownie wynosi 5 i jest mutowalny
```

### 4.4. Wbudowane wejście i wyjście (I/O)

Język dostarcza dwie funkcje `read` i `write`, o sygnaturach `string read()` i `void write(T)`. Funkcja `read` jest bezargumentowa i wczytuje dane użytkownika wpisane w terminalu; zwraca zawsze typ `string`. Funkcja `write` może przyjąć jeden argument, dowolnego typu, ponieważ użytkownik nie może dfiniować własnych typów zachowanie zawsze jest znane i nic nie zwraca. Funkcja `write` nie przesuwa kursora automatycznie do następnej linii, należy używać manualnie `'\n'`.

Przykłady użycia:

```Rusthon++
string name = read()               # wprowadzamy "Jan"
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

### 5. Raportowanie błędów i diagnostyka

Stosuje się strategię "fail-fast". Oznacza to, że po napotkaniu błędu na dowolnym etapie, program kończy się błędem.

### 5.1. Typy zgłaszanych błędów

#### 5.1.1. Błędy leksykalne

Wykrywane przez lekser podczas tokenizacji.

| Rzucony wyjątek | Przyczyna |
| :--- | :--- |
| FloatLiteralOutOfBoundsException | Przekroczenie granicy wartości typu zmiennoliczbowego |
| IntLiteralOutOfBoundsException | Przekroczenie granicy wartości typu całkowitego |
| InvalidCharLiteralException | Niepoprawny format literału znakowego (np. puste apostrofy `''` lub zbyt wiele znaków) |
| InvalidCommentStyleException | Miedozwolony styl komentarza /**/ |
| MalformedNumericLiteralException | Zniekształcony / niepoprawnie sformatowany literał liczbowy (np. `0xG1` lub `1.2.3`) |
| TooLongIdentifierException | Przekroczenie maksymalnej dopuszczalnej długości identyfikatora (nazwy) |
| TooLongStringLiteralException | Przekroczenie maksymalnej dopuszczalnej długości literału tekstowego |
| UnknownEscapedCharacterException | Użycie nieznanej sekwencji ucieczki (np. `\z` wewnątrz tekstu) |
| UnknownSymbolException | Napotkanie nieznanego lub niedozwolonego symbolu w kodzie źródłowym, np. `;` |
| UnterminatedCharLiteralException | Niedomknięty literał znakowy (brak kończącego apostrofu przed końcem linii/pliku) |
| UnterminatedStringLiteralException | Niedomknięty literał tekstowy (brak kończącego cudzysłowu przed końcem linii/pliku) |

#### 5.1.2. Błędy składniowe

Wykrywane przez parser podczas budowania drzewa AST z tokenów.

| Rzucony wyjątek | Przyczyna |
| :--- | :--- |
| InvalidParameterException | Niepoprawny parametr (np. w definicji lub wywołaniu funkcji) |
| InvalidTypeException | Niepoprawny lub nierozpoznany typ danych |
| MissingBracketException | Brakujący nawias kwadratowy `[` / `]` |
| MissingExpressionException | Brakujące wyrażenie (np. wewnątrz instrukcji warunkowej lub po znaku przypisania) |
| MissingIdentifierException | Brakujący identyfikator (np. brak nazwy zmiennej lub funkcji w miejscu deklaracji) |
| MissingKeywordException | Brakujące słowo kluczowe języka (np. `if`, `while`, `return`) |
| MissingNewlineException | Brakujący znak nowej linii (jeśli gramatyka wymaga go do zakończenia instrukcji) |
| MissingOperatorException | Brakujący operator między wyrażeniami lub operandami |
| MissingParenthesisException | Brakujący nawias okrągły `(` lub `)` |
| MissingRightOperandException | Brakujący prawy operand w wyrażeniu dwuargumentowym |
| NotConsumedTokensException | Nadmiarowe, nieskonsumowane tokeny po zakończeniu parsowania głównej struktury programu (śmieci na końcu pliku) |

#### 5.1.3. Błędy semantyczne i wykonania

Wykrywane przez interpreter podczas analizy semantycznej i wykonywania kodu. Wyjątki rzucane są z rodziny `InterpreterException`.

| Rzucony wyjątek | Przyczyna |
| :--- | :--- |
| FunctionSignatureMismatchException | Przekazanie niepoprawnej liczby lub typów argumentów do funkcji (niezgodność z jej sygnaturą) |
| VoidValueException | Próba użycia wartości typu `void` w wyrażeniu (np. przypisanie wyniku funkcji niezwracającej wartości) |
| NotAllowedTypeException | Użycie typu danych niedozwolonego w danym kontekście operacji |
| InvalidCastException | Niepoprawna lub niemożliwa konwersja (rzutowanie) między typami danych |
| InvalidOperationException | Próba wykonania niedozwolonej operacji (np. użycie operatora niedopasowanego do typów w czasie wykonywania) |
| IndexOutOfBoundsException | Wyjście poza dopuszczalny zakres indeksu kolekcji, tablicy lub napisu |
| InvalidAccessException | Próba nielegalnego lub niepoprawnego dostępu do składowej, zmiennej lub obszaru pamięci |
| InvalidOverloadException | Niepoprawne lub konfliktowe przeciążenie funkcji wykryte w czasie interpretacji |
| InvalidShadowException | Niepoprawne lub niedozwolone przesłonięcie (shadowing) identyfikatora w zagnieżdżonym bloku |
| UnknownIdentifierException | Odwołanie do niezadeklarowanego lub nieznanego identyfikatora (zmiennej, funkcji) podczas wykonywania programu |
| EmptyAccumulatorException | Próba odczytu wartości z pustego akumulatora (błąd wewnętrzny stanu rejestru/maszyny wirtualnej interpretera) |
| BuiltinFunctionInitException | Błąd podczas rejestracji lub inicjalizacji wbudowanej funkcji (funkcji API interpretera) |
| InvalidStatementException | Napotkanie i próba wykonania niepoprawnej logicznie lub strukturalnie instrukcji w czasie runtime |
| StackOverflowException | Przepełnienie stosu wywołań interpretera (najczęściej spowodowane zbyt głęboką lub nieskończoną rekurencją) |

### 5.2. Przykładowe komunikaty o błędzie

Błędy leksykalne, składniowe i semantyczne podają po prostu lokalizację błędu i przyczynę.
Błędy wykonania podają też traceback, aby ułatwić debugowanie.

Błąd leksykalny:

```Rusthon++
"abc
```

```plaintext
Error: Lexer Exception at Line: 1, Column: 1
Unterminated string literal "a...". Expected closing quote before the end of the line. (Note: Rusthon++ strings cannot span multiple lines)
                             ~~~^
```

Błąd składniowy (brakujący token):

```Rusthon++
if (true)
    write("hello")
done
```

```plaintext
Error: Parser Exception at Line: 1, Column: 10
Missing Keyword 'KW_DO' when building ''do ... done' block'.
```

Błąd semantyczny (próba zmiany stałej):

```Rusthon++
int x = 5
x = 3
```

```plaintext
Error: Interpreter Exception at Line: 2, Column: 1
cannot assign to immutable value
```

## 6. Narzędzia i cykl życia

### 6.1. Narzędzia (komponenty)

#### Proces analizy i wykonania kodu

* Interpreter żąda od parsera drzewa AST
* Parser czyta strumień tokenów, aż sformułuje całe drzewo AST
* Na żądanie parsera lekser czyta strumień znaków i przygotowuje po jednym tokenie
* Interpreter odwiedza drzewo i je wykonuje

Wyjątkami są funkcje i bloki, które muszą być wczytane w całości naraz.

#### Przykrywanie zmiennych

Tabela zmiennych jest dwuwymiarowa (1-zmienne, 2-zakresy). Gdy tworzony jest nowy blok, np. ciało funkcji zostaje dodany nowy wymiar (efektywnie push na stos). Interpreter szukając identyfikatora przegląda tabelę od najwyższej pozycji (najnowszego zakresu). Gdy blok się kończy, cały wymiar zostaje odrzucony, przez co garbage collector nie jest potrzebny.

### 6.2. Sposób uruchomienia

```plaintext
interpreter script.txt
```

Zakończenie sesji: `exit()`

Wbudowana funkcja `exit()` służy do zakończenia działania programu i może być również użyta w kodzie.
