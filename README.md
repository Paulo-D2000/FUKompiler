# FUKompiler
FUKompiler is a basic (Toy) C compiler written in C++

### [WARNING] Tested only with MSVC...

### Usage [*EXPERIMENTAL*]:
To 'Build & Run' a test :

```FUK ../example/helloworld.c > output.py```\
```python3 ./output.py```

#### Limitations:
- *Python3 Target only*
- *AST Debug Output* 

### Supports: (Grammar I gues?)
- Literals: [int] `0`, [float] `1.0`, [char] `'c'`, [string] `"text"`
- Keywords: `return`, `void`, `int`, `char`, `float`
- Expressions: 
    - [LitIdent] (`Literal` | `Identifier`)
        - Literal:
            - [int] `0`, [float] `1.0`, [char] `'c'`, [string] `"text"`
        - Identifier:
            - variable, function_name
    - [UnaryOp] (`Op`,`Expression`, `Op`)
        - Op:
            - Paren `(`,`)`
    - [BinaryOp] (`Expression`, `Op`, `Expression`)
        - Op:
            - Add `+`
            - Sub `-`
            - Mul `*`
            - Div `/`
            - Lt `<`
            - Gt `>`
            - LtEq `<=`
            - GtEq `>=`
            - Eq `==`
- Statements:
    - FuncCall Statemt: `FunctionName:{Identifier} (ArgType:Keyword ArgName:Identifier);`\
    which equals to:
        ```cpp
        printf("Text");
        ```
    - Return Statement: `return{Kewyord} {Expression};`\
    which equals to:
        ```cpp
        return 0;
        ```
    - VarDecl Statement: `VarType:{Keyword} VarName:{Identifier} = {Expression};`\
    which equals to:
        ```cpp
        float value = 1.0 + 2.0f;
        ```
- Functions with the signature:
    ```
    RetType:Keyword FuncionName:Identifer (ArgType:Keyword ArgName:Identifier, ArgType:Keyword ArgName:Identifier, ...) {
        return:Kewyord {Expression};
    }
    ```
    which equals to:

    ```cpp
    int add (int a, int b) {
        return a + b;
    }
    ```
### Building
Deps: `Cmake, C++ Compiler (Visual Studio C++, MinGW W64 GCC, GCC)`
Run:
```
mkdir build
cd build
cmake ..
cmake --build .
````

### TODO:
- Update TODO...
- Split Parser into .h and .cpp
- Handle a bigger subset of the C Programming Language
- Add more (actual) tests
- Implement Error Checking
- Implement Variable lookup, Stack, etc
- Add IR ?
- Assembly or Binary output
- Add Optimizing ? (Needs IR ?)