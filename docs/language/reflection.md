# Reflections

## `value` Keyword

`value` keyword can be used to create a value-type object. (e.g. `value (0, 1)`)

A value-type object has no runtime overhead, but you can't use runtime features like dynamic type check and reflection.

Note that `value` keyword doesn't mean that the objects without `value` are not value-type object. All objects are 
value-type objects by default in Seserot, but they have an 8 bytes header, which is used to store the type information.

