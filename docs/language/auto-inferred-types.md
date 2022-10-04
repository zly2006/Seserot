# Auto-Inferred Types

There are some special grammar patterns that is used to create an object.
This is an instruction for how the compiler create this kind of objects and what the objects' types are.
For all of them, you can make it to have no [type header](reflection.md#value-keyword) by adding the keyword `value`,
e.g. `value (obj1, obj2, ...)`.

## Tuple

`(obj1, obj2, ...)` will be inferred as `seserot.Tuple<type1, type2, ...>`.

There will be no runtime overhead for value-type tuple.

e.g. `(0, "")`'s type is `Tuple<Int, String>`

## List

`[obj1, obj2, ...]` will be inferred as `seserot.List<type1, type2, ...>`.

e.g. `[0, 1]`'s type is `List<Int, String>`, and `[1, ""]`'s type is `List<Object>`.

## Anonymous Type

`{ a : 1 , b : 1}` will be inferred as

```
class <Anonymous> {
    val a: Int
    val b: Int
}
```

