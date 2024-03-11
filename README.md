# FUKompiler
FUKompiler is a basic C compiler written in C++


### Usage Example :
```FUK ..\example\test.c```

### Supports:
- Literals: [int] `0`, [char] `'c'`, [string] `"text"`
- Keywords: `return`
- Identifiers (Types): `int`, `char`
- Variables (definition): `Identifier varname = Literal;`
- Functions with the signature:
```cpp
Identifier FuncName() {
    ...
    return Literal;
}
```

#### *EXPERIMENTAL -> Python3 Target only*

```FUK ..\example\test.c > output.py```\
```python3 ./output.py```