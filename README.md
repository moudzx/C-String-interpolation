# rprintf
Rust-style string interpolation for C -- use {variable} directly in format strings with automatic type detection via _Generic.

 
```c
int age = 26;
char *name = "Moss";
watch(age);
watch(name);
 
rprintf("hello {name}, you are {age} years old\n");
```

## Why `watch` is needed
 
C variable names are erased by the compiler. after compilation, `age` is just a stack offset like `[rbp-4]`. There is no runtime table mapping names to values unless you build one explicitly. `watch(age)` uses the `#var` preprocessor stringification operator to capture the name `"age"` and snapshot the current value into the registry.

## How it works
 
**Type detection** : `_Generic` dispatches each variable to the correct `make_*` constructor at compile time, capturing both the value and its type tag into an `Arg` tagged union.
 
**Array length** : `sizeof(arr)/sizeof(arr[0])` is evaluated inside `watch` before the array decays to a pointer, so no sentinel value is needed.
 
**Registry** : a static `VarRegistry` of up to 256 `(name, Arg)` pairs. `watch` snapshots the current value on each call.
 
**Interpolation** : `rprintf` walks the format string, extracts names between `{` and `}`, looks them up in the registry, and prints via `print_arg`.

## Limitations
 
- Registry is global and capped at 256 entries
- `watch` must be called manually after each mutation
- Supported array types: `int[]` and `double[]`
- Not thread-safe
 
