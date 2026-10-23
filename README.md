# 基于递归自顶向下的语法分析器

## 项目简介

本项目是一个用 C 语言实现的简单类 C 语言词法分析与递归下降语法分析程序。程序先读取源文件并完成词法分析，将关键字、标识符、整数、运算符和界符转换为 Token 序列；随后根据预先定义的文法，为各个非终结符建立对应的递归分析函数，判断输入程序的语法是否正确。

本实验完成了词法单元识别、Token 流传递、声明与语句分析、算术与关系表达式分析、控制结构分析和常见语法错误报告。分析过程会输出当前 Token 及 Token 读取过程，便于观察递归调用和排查语法错误。

## 文件说明

| 文件 | 作用 |
| --- | --- |
| `main.c` | 程序入口，读取待分析文件名，依次调用词法分析和语法分析。 |
| `TESTscan.c` | 词法分析器，识别 Token，检测非法字符并记录行号。 |
| `TESTparse.c` | 递归下降语法分析器，实现各非终结符的分析函数和错误报告。 |
| `test.txt` | 默认测试源程序。 |
| `start.bat` | Windows 批处理脚本，自动编译并将 `test.txt` 作为输入文件名传给程序。 |
| `output/main.exe` | GCC 编译生成的可执行文件。 |

## 词法分析

词法分析器目前可识别：

- 关键字：`if`、`else`、`while`、`for`、`do`、`int`、`read`、`write`。
- 标识符：以字母开头，后续由字母或数字组成，Token 类型为 `ID`。
- 无符号十进制整数，Token 类型为 `NUM`。
- 算术运算符：`+`、`-`、`*`、`/`。
- 关系和赋值运算符：`<`、`>`、`<=`、`>=`、`==`、`!=`、`=`。
- 界符：`(`、`)`、`{`、`}`、`,`、`:`、`;`。
- C 风格块注释：`/* ... */`。

如果发现未定义字符，词法分析器会输出非法字符及其所在行，并终止后续分析。

## 主要文法

```text
program          -> { declaration_list statement_list }
declaration_list -> declaration_stat declaration_list | ε
declaration_stat -> int ID ;
statement_list   -> statement statement_list | ε
statement        -> if_stat | while_stat | for_stat | read_stat
                    | write_stat | compound_stat | expression_stat
compound_stat    -> { statement_list }
expression_stat  -> expression ; | ;
expression       -> ID = bool_expr | bool_expr
bool_expr        -> additive_expr [ relop additive_expr ]
additive_expr    -> term [ (+ | -) term ]
term             -> factor [ (* | /) factor ]
factor           -> ( expression ) | ID | NUM
if_stat          -> if ( expression ) statement [ else statement ]
while_stat       -> while ( expression ) statement
for_stat         -> for ( expression ; expression ; expression ) statement
read_stat        -> read ID ;
write_stat       -> write expression ;
```

每个分析函数返回时，全局变量 `token` 和 `token1` 保存下一个待处理 Token 的类型和值。调用者应先检查当前 Token，只在确认已消耗该 Token 后才继续读取，以避免跳过分号、右括号或右花括号。

## 编译与运行

### 环境要求

- Windows
- GCC（例如 MinGW-w64）
- `gcc` 已加入系统 `PATH`

### 使用批处理脚本

双击 `start.bat`，或在项目目录中执行：

```powershell
.\start.bat
```

脚本实际执行的主要命令为：

```bat
gcc "main.c" -o "output\main.exe"
echo test.txt | "output\main.exe"
```

由于 `main.c` 已包含 `TESTparse.c`，而 `TESTparse.c` 又包含 `TESTscan.c`，因此只需编译 `main.c`。不要再把 `TESTscan.c` 单独加入同一条编译命令，否则会出现函数和全局变量重复定义的链接错误。

### 手动运行

```powershell
gcc main.c -o output\main.exe
.\output\main.exe
```

程序提示输入文件名时，输入：

```text
test.txt
```

## 测试说明

一个可通过语法分析的示例为：

```c
{
    int i;
    int n;
    int j;
    j = 1;
    read n;
    for (i=0; i<n; i=i+1)
        j = j+1;
    write j;
}
```

当前 `test.txt` 中的 `for` 语句故意缺少右括号 `)`，用于验证语法错误检测。运行后应报告括号不匹配，而不是语法分析成功。

## 错误处理

程序目前可报告的主要问题包括：

- 源文件无法打开。
- 词法分析发现非法字符。
- 程序最外层缺少或多出花括号。
- 语句结尾缺少分号。
- 表达式结构错误。
- `if`、`while` 或 `for` 后缺少左括号。
- 圆括号不匹配。
- 声明或 `read` 语句中需要标识符的位置出现关键字。

语法分析器还会检查文件结束位置：若最外层右花括号前已到达文件末尾，或最外层右花括号后仍有多余 Token，则报告花括号不匹配。

## 已知不足

- 递归下降方法易于理解，但频繁的函数调用会带来额外时间和栈空间开销，深度嵌套时可能出现栈溢出。
- 发现第一个词法或语法错误后会直接终止，尚未实现基于同步 Token 的错误恢复，不能跳过错误后继续报告其他问题。
- 语法错误信息主要描述错误类型，尚未完整记录错误行号、列号和调用链。
- Token 数组容量为 256，当前未对超长输入做动态扩容。
- 仅支持字母开头的标识符和十进制整数，不支持下划线标识符、浮点数、字符串、`//` 行注释以及宏定义。
- `do` 可被词法分析器识别为关键字，但语法分析器尚未实现 `do-while` 语句。
- 本项目只进行词法和语法检查，不包含类型检查、符号表管理、中间代码生成或程序执行。
