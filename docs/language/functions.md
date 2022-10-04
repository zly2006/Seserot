# Functions

## Types of Functions

### Normal Function

```
function add1(v1: Int): Int {
    return v1 + 1
}
```

### Member Function

```
class T{
    function foo(): String {
        return "This is a memeber function"
    }
}
```

### Pure Function

```
pure function add1(v1: Int): Int {
    return v1 + 1
}
```

The keyword `pure` tells the compiler that this function's return value only depends on its args, and there will be no side-effect while calling this function.

Pure functions can only call pure functions. If not, this will cause a CE.

Getters of the properties is pure by default.

### Getter/Setter

```typescript
class A{
    var mutableVar = 0 { get = field set = value -> { field = value } }
}
```

A function `function a() = aVariable` means that this function always return `aVariable`'s value.

A `var` has both getter & setter, and a `val` only has getter.

### Infix

```
infix function A.fun(b: B) {
/* Or:
infix function fun(a: A, b: B) {
*/
	//...
}
```

Call:

```
var a = new A()
var b = new B()
a fun b
```

An infix function must have 2 params.

## A Function is an object

### Type of Function

`[DeclaredType].(ParaType1, ParaType2, ...) -> RetType`

e.g.

```
class A {
    function foo(&a: Int): String {
        return "a is $a"
    }
}
```

The type of `foo` is `A.(Int) -> String`

### Operators between Functions

If `f2`'s para(only one) type is `f1`'s return type, you can composite them:

`function qwq = f2 * f1`  means `qwq(x)` is `f2(f1(x))`.

If `f1`'s type is the same as 'f2', then you can use `+` to create a new function:

`function awa = f1 + f2` means
```
function awa(arg: AnyType) {
    f1(&arg)
    f2(&arg)
}
```

You can also specify some args to create a new function:

```
function max(a: Int, b: Int): Int {...}

function maxWith5 = max(_, 5)
```

Operators are functions, so you can:

`function add1 = _ + 1`