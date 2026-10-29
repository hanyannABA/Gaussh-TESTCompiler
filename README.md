# Non-recursive LL(1) Parser for Expression Grammar

本目录实现了一个基于预测分析表的非递归 LL(1) 表达式语法分析器。程序先进行词法分析，再使用显式分析栈完成自顶向下语法分析。完整版还会构造并输出分析树（Parse Tree）。

## 文法

程序使用以下消除左递归后的表达式文法：

```text
E  -> T E'
E' -> + T E' | epsilon
T  -> F T'
T' -> * F T' | epsilon
F  -> ( E ) | ID | NUM
```

该文法保证乘法 `*` 的优先级高于加法 `+`，并支持使用括号改变计算顺序。输入末尾由词法分析器自动添加 `$` 作为结束标记。

## 文件说明

| 文件 | 作用 |
| --- | --- |
| `EFTF_LL_with_grammar_tree.c` | 额外版本；在主要版本的基础上AI辅助增加语法树(真不会画) |
| `ETF_LL.c` | 主要版本；完成词法分析、非递归 LL(1) 分析、分析树构造与输出 |
| `TESTscan.c` | 词法分析器；将 Token 保存到 `Scanout` 数组并输出词法分析结果 |
| `EFT_test.txt` | 默认测试输入：`id+id*(id+id)`，也可改为包含数字的表达式 |
## 实现流程

1. `TESTscan()` 读取输入文件，识别 `ID`、`NUM`、`+`、`*`、`(` 和 `)` 等 Token。
2. Token 按顺序保存在 `Scanout` 数组中，末尾追加 `$`。
3. `LLparsing()` 使用符号栈执行非递归 LL(1) 分析。
4. `treeStack` 与符号栈同步，保证每个待处理文法符号都对应正确的树节点。
5. 分析成功后，`printTree()` 以缩进形式输出完整分析树，`freeTree()` 递归释放动态内存。

## 编译

在 PowerShell 中进入仓库根目录，然后使用 GCC 编译带分析树的主程序：

```powershell
gcc ".\EFTF_LL_with_grammar_tree.c" -std=c11 -Wall -Wextra -pedantic -o ".\EFTF_LL_with_grammar_tree.exe"
```

`EFTF_LL_with_grammar_tree.c` 已直接包含 `TESTscan.c`，因此编译时不要再把 `TESTscan.c` 作为独立源文件加入命令，否则会出现重复定义错误。

## 运行

```powershell
.\EFTF_LL_with_grammar_tree.exe
```

程序提示输入文件名时，输入：

```text
EFT_test.txt
```

默认测试表达式为：

```text
id+id*(id+id)
```

数字表达式也可直接分析，例如：

```text
5+4*(3+7)
```

## 输出

程序会依次显示：

1. 词法分析得到的 Token 类型和值。
2. LL(1) 分析过程中的符号栈状态。
3. `Parsing succeeded!!!` 或 `Syntax error!!!`。
4. 分析成功时的完整 Parse Tree。

分析树输出示例：

```text
|-- E
    |-- T
        |-- F
            |-- ID
        |-- T'
            |-- epsilon
    |-- E'
        |-- +
        |-- T
            ...
```

## 已知限制

- 语法分析部分仅支持上述表达式文法，不支持完整程序语句。
- Token 数组长度固定为 256，分析栈长度固定为 1024。
- 分析树保留 `E'`、`T'` 和 `epsilon` 等文法节点，因此输出的是 Parse Tree，不是简化后的 AST。
- `TESTscan.c` 中的 `TESTout()` 和 `ERROR()` 目前在首次调用前没有显式函数原型，使用严格警告选项时会出现警告，但当前 MinGW GCC 可生成可执行文件。
