# wheelcc - the C compiler reinventing the wheel

> **THIS COMPILER IS A WORK IN PROGRESS! ANYTHING CAN CHANGE AT ANY MOMENT WITHOUT ANY NOTICE! USE THIS COMPILER AT YOUR OWN RISK!**

```
  ██╗       ██╗ ██╗        ██████╗   ██████╗  ██╗
  ██║       ██║ ██║       ██╔═══██╗ ██╔═══██╗ ██║        ███████╗  ███████╗
  ██║  ██╗  ██║ ███████╗  ████████║ ████████║ ██║       ██╔═════╝ ██╔═════╝
  ██║  ██║  ██║ ██╔═══██╗ ██╔═════╝ ██╔═════╝ ██║       ██║       ██║
  ╚████╔═████╔╝ ██║   ██║ ╚███████╗ ╚███████╗ ╚███████╗ ╚███████╗ ╚███████╗
   ╚═══╝ ╚═══╝  ╚═╝   ╚═╝  ╚══════╝  ╚══════╝  ╚══════╝  ╚══════╝  ╚══════╝
```

*__<ins>Reinventing the wheel</ins>__ (idiom): "Waste a great deal of time or effort in creating something that already exists."*
<!---->
A small, self-contained C compiler written from scratch in C++ for x86-64 GNU/Linux platforms. 

****

The wheelcc C compiler supports a large subset of C17 (International Standard ISO/IEC 9899:2018), for which it has it's own built-in preprocessor, frontend, IR, optimization and backend. It emits x86-64 AT&T assembly for GNU/Linux, which is then linked with gcc/ld. wheelcc is written in C++, and builds to a standalone executable + a bash driver.

## 2025 Roadmap

Next development milestones planned for this year:  
- [x] Add IR optimizations
- [x] Add a register allocator
- [ ] Clean up the codebase
- [ ] Migrate the compiler to C
- [ ] Support MacOS
- [ ] Start an experimental stdlib

## Migrating to C

(Starting soon) A C compiler should be written in C, right? wheelcc was implemented from the start with the goal to switch to plain C at some point. C++ was used for development for its very handy std that made prototyping and large-scale refactoring much easier. The use of C++ features has been kept to a minimum and restricted to a small subset of the language, and almost all the code sticks already close to C-style. Now that the compiler has grown to a decent size and a stable architecture, it is finally time to migrate to C! The migration is done on branch `cpp2c/develop` until complete.

### Progress

- [x] `all` Remove easely removable C++ features (classes, templates, overloads, namespaces, ...)
- [ ] `lexer` Replace C++ regex library with a C alternative (either POSIX regex or regexp9)
- [ ] (POC done) `error handler` Replace exception throwing with error code propagation
- [ ] (POC done) `algebraic datatypes` Replace single inheritance data structures with tagged unions
- [ ] (POC done) `polymorphism` Replace smart pointers with manual memory management and reference count
- [ ] (POC done) `std containers` Replace collections and strings with C alternatives (stb_sd and sds)

## Usage

### Distros

The tip of `master` branch has passed all tests and validation for these GNU/Linux distributions (x86-64):  
Debian GNU/Linux 12|Linux Mint 22|Ubuntu 22.04.5 LTS|openSUSE Leap 15.6|Rocky Linux 9.5|Arch Linux|EndeavourOS Linux
:---:|:---:|:---:|:---:|:---:|:---:|:---:
:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:

### Install

- Get the repo, cd to the bin directory  
```
$ git clone --depth 1 --branch master https://github.com/romainducrocq/wheelcc.git
$ cd wheelcc/bin/
```
- Configure the repo and install the build/runtime dependencies: `gcc g++ make cmake`  
```
$ ./configure.sh
```
- Build the compiler in Release mode  
    > requires `$ gcc -dumpfullversion` >= 8.1.0
```
$ ./make.sh
```
- Install the `wheelcc` command system-wide (creates a symlink to the driver in `/usr/local/bin/`)  
    > or, do not install system-wide and use `bin/driver.sh` directly instead  
```
$ ./install.sh
$ . ~/.bashrc
```

### Use

With file `main.c`  
```c
int puts(char* c);

int main(void) {
    puts("Hello, world!");
    return 0;
}
```

- Compile and run  
```
$ wheelcc main.c
$ ./main
Hello, World!
```

- Usage  

All command-line arguments are optional, except for one `.c` source file to compile.
> **Warning**: <ins>The order of command-line arguments matters!</ins> They are parsed in the order shown by `--help` (and only in that order). Passing arguments in any other order will fail with an `unknown or malformed option` error.
```
$ wheelcc --help
Usage: wheelcc [Help] [Debug] [Optimize...] [Preprocess] [Link] [Include...]
 [Linkdir...] [Linklib...] [Output] FILES

[Help]:
    --help  print help and exit

[Debug]:
    -v          enable verbose mode
    (Test|Debug build only):
    --lex       print  lexing    stage and exit
    --parse     print  parsing   stage and exit
    --validate  print  semantic  stage and exit
    --tacky     print  interm    stage and exit
    --codegen   print  assembly  stage and exit
    --codeemit  print  emission  stage and exit

[Optimize...]:
    (Level 0):
    -O0                           disable  all optimizations
    (Level 1):
    --fold-constants              enable   constant folding
    --propagate-copies            enable   copy propagation
    --eliminate-unreachable-code  enable   unreachable code elimination
    --eliminate-dead-stores       enable   dead store elimination
    --optimize                    enable   all level 1 optimizations
    -O1                           alias    for --optimize
    (Level 2):
    --no-allocation               disable  register allocation
    --no-coalescing               disable  register coalescing
    --allocate-register           enable   register allocation (default)
    -O2                           alias    for --allocate-register
    (Level 3):
    -O3                           alias    for -O1 -O2

[Preprocess]:
    -E  enable macro expansion with gcc

[Link]:
    -s  compile, but do not assemble and link
    -c  compile and assemble, but do not link

[Include...]:
    -I<includedir>  add a list of paths to include path

[Linkdir...]:
    -L<linkdir>  add a list of paths to link path

[Linklib...]:
    -l<libname>  link with a list of library files

[Output]:
    -o <file>  write the output into <file>

FILES:  list of .c files to compile
```

### Errors

Compile errors output messages with file, line and explanation to stderr:  
```c
int main(void) {
    int i = { 1 };
    return 0;
}
```
```
$ wheelcc main.c
/home/user/wheelcc/main.c:2:
error: (no. 547) cannot initialize scalar type ‘int’ with compound initializer
at line 2:     int i = { 1 };
wheelcc: error: compilation failed
```

### Test

- cd to the test directory, get the testtime dependencies: `diffutils valgrind`  
```
$ cd test/
$ ./get-dependencies.sh
```

- Test the compiler  
    > requires `$ gcc -dumpfullversion` >= 8.1.0
```
$ ./test-compiler.sh [-O0 | -O1 | -O2 | -O3]
```

- Test the preprocessor  
```
$ ./test-preprocessor.sh
```

- Test error handling  
```
$ ./test-errors.sh
```

- Test memory leaks  
```
$ ./test-memory.sh [-O0 | -O1 | -O2 | -O3]
```

- Run all tests  
```
$ ./test-all.sh
```

## Compiler overview

### Preprocessor

A minimal built-in preprocessor supports `include` header directives and comments (singleline and multiline). By default, included files are searched in the same directory as the source file currently being compiled, but other directories to search for can be added to the include path with the `-I` option. Other directives, like pragmas, are ignored and stripped out.  
The preprocessor does not natively support macros, but macro expansion can be enabled with the `-E` command-line option, which falls back on preprocessing with gcc.

### Compiler

wheelcc compiles a list of C source files to x86-64 AT&T GNU/Linux assembly (see [_Implementation Reference_](https://github.com/romainducrocq/wheelcc/tree/master?tab=readme-ov-file#implementation-reference) section for a list of supported C language features).
The `-s` command-line option can be used to output the assembly without linking, and the `-c` option to create an object file instead of an executable. Otherwise, it creates an executable located next to the first source file and with the same name without the extension, or with the name set with the `-o` command-line option.  
wheelcc also has comprehensive compile error handling, and outputs error messages with the file, line and explanation for the compile error to stderr.

### Optimization

wheelcc can perform multiple compiler performance optimizations for smaller and faster assembly outputs. The level 1 `-O1` command-line option enables all IR optimizations: constant folding, unreachable code elimination, copy propagation and dead store elimination. The level 2 `-O2` (TBD) command-line option enables backend register allocation with coalescing (but it does not enable level 1 optimizations). The `-O3` option enables all optimizations (level 1 and 2) and the `-O0` option disables them all. By default, only `-O2` is enabled.

### Linker

There is no built-in linker, the compiler outputs assembly that is then linked with gcc/ld. That output follows the System-V ABI, which allows to link other libraries pre-compiled with gcc (or other compilers) with the `-L` and `-l` command-line options and use them at runtime in a program compiled by wheelcc. This also allows to link the C standard library method APIs which declarations are supported by the current implementation of wheelcc.  

### Standard library

(TBD, an experimental standard library is planned in the future, with at least support for the compiler tests.)

### Dependencies

wheelcc aims to be self-contained and as less bloated as possible. It only depends on the C and C++ standard libraries and a few file-only dependencies that are already included in the sources ([ctre](https://github.com/hanickadot/compile-time-regular-expressions), [boost::regex](https://github.com/boostorg/regex), [tinydir](https://github.com/cxong/tinydir)). The build+runtime only requires bash, gcc/g++ (>= 8.1.0) and cmake, which makes the compiler easy to build and use on any x86-64 GNU/Linux platform.

### Limitations

The compiler supports a large subset of the C17 language, but many features of the language are still not implemented. These include, but are not limited to: enum data structures, variable-length arrays, const types, typedefs, function pointers, non-ascii characters, and float, short, auto, volatile, inline, register and restrict keywords. Any of these may or may not be implemented in the future. As such, wheelcc can not compile the C standard library and is not intended to be used as a production C compiler.

## Implementation reference

Here's everything the wheelcc compiler supports from the C17 language (so far). For code examples, see:
- `test/tests/compiler/`  
- `test/tests/preprocessor/`  

### Development

**Preprocessor**

- [x] [Include directives, comments](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/preprocessor/preprocessor)

**Language features**

- [x] [Return, integer constants](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/1_int_constants)
- [x] [Unary arithmetic operators](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/2_unary_operators)
- [x] [Binary arithmetic operators](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/3_binary_operators)
- [x] [Bitwise arithmetic operators](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/3_binary_operators/valid/extra_credit)
- [x] [Logical and relational operators](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/4_logical_and_relational_operators)
- [x] [Local variables, assignments](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/5_local_variables)
- [x] [Compound assignments, increment operators](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/5_local_variables/valid/extra_credit)
- [x] [If statements, conditional expressions](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/6_statements_and_conditional_expressions)
- [x] [Goto statements, labels](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/6_statements_and_conditional_expressions/valid/extra_credit)
- [x] [If if-else else compound statements](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/7_compound_statements)
- [x] [While, do while and for loops](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/8_loops)
- [x] [Switch statements](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/8_loops/valid/extra_credit)
- [x] [Functions](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/9_functions)
- [x] [File-scope variables, static and extern storage-class specifiers](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/10_file-scope_variables_and_storage-class_specifiers)

**Types**

- [x] [Integers, long integers](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/11_long_integers)
- [x] [Signed and unsigned integers](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/12_unsigned_integers)
- [x] [Double floating-point numbers, nan](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/13_floating-point_numbers)
- [x] [Pointers](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/14_pointers)
- [x] [Fixed-sized arrays, pointer arithmetic](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/15_arrays_and_pointer_arithmetic)
- [x] [Characters, strings literals, ascii](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/16_characters_and_strings)
- [x] [Void, support dynamic memory allocation](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/17_supporting_dynamic_memory_allocation)
- [x] [Structures](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/18_structures)
- [x] [Unions](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/18_structures/valid/extra_credit)

**Optimization**

- [x] [IR optimization pipeline](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/19_optimizing_three_address_code_programs/whole_pipeline)
- [x] [Constant folding](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/19_optimizing_three_address_code_programs/constant_folding)
- [x] [Unreachable code elimination](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/19_optimizing_three_address_code_programs/unreachable_code_elimination)
- [x] [Copy propagation](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/19_optimizing_three_address_code_programs/copy_propagation)
- [x] [Dead store elimination](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/19_optimizing_three_address_code_programs/dead_store_elimination)
- [x] [Register allocation](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/20_register_allocation)
- [x] [Register allocation with coalescing](https://github.com/romainducrocq/wheelcc/tree/master/test/tests/compiler/20_register_allocation/all_types/with_coalescing)

### Language grammar

```
<param> ::= { <type-specifier> }+ <declarator>
<simple-declarator> ::= <identifier> | "(" <declarator> ")"
<type-specifier> ::= "int" | "long" | "unsigned" | "signed" | "double" | "char" | "void" 
                   | ("struct" | "union") <identifier>
<specifier> ::= <type-specifier> | "static" | "extern"
<block> ::= "{" { <block-item> } "}"
<block-item> ::= <statement> | <declaration>
<initializer> ::= <exp> | "{" <initializer> { "," <initializer> } [ "," ] "}"
<for-init> ::= <variable-declaration> | [ <exp> ] ";"
<statement> ::= "return" [ <exp> ] ";" | <exp> ";" 
              | "if" "(" <exp> ")" <statement> [ "else" <statement> ] | "goto" <identifier> ";"
              | <identifier> ":" | <block> | "break" ";" | "continue" ";"
              | "while" "(" <exp> ")" <statement> | "do" <statement> "while" "(" <exp> ")" ";"
              | "for" "(" <for-init> [ <exp> ] ";" [ <exp> ] ")" <statement>
              | "switch" "(" <exp> ")" <statement> | "case" <const> ":" <statement>
              | "default" ":" <statement> | ";"
<exp> ::= <cast-exp> | <exp> <binop> <exp> | <exp> "?" <exp> ":" <exp>
<cast-exp> ::= "(" <type-name> ")" <cast-exp> | <unary-exp>
<unary-exp> ::= <unop> <cast-exp> | "sizeof" <unary-exp> | "sizeof" "(" <type-name> ")" 
              | <postfix-exp>
<type-name> ::= { <type-specifier> }+ [ <abstract-declarator> ]
<postfix-exp> ::= <primary-exp> { <postfix-op> }
<postfix-op> ::= "[" <exp> "]" | "." <identifier> | "->" <identifier>
<primary-exp> ::= <const> | <identifier> | "(" <exp> ")" | { <string> }+ 
                | <identifier> "(" [ <argument-list> ] ")"
<argument-list> ::= <exp> { "," <exp> }
<abstract-declarator> ::= "*" [ <abstract-declarator> ] | <direct-abstract-declarator>
<direct-abstract-declarator> ::= "(" <abstract-declarator> ")" { "[" <const> "]" } 
                               | { "[" <const> "]" }+
<unop> ::= "-" | "~" | "!" | "*" | "&" | "++" | "--"
<binop> ::= "-" | "+" | "*" | "/" | "%" | "&" | "|" | "^" | "<<" | ">>" | "&&" | "||" | "==" 
          | "!=" | "<" | "<=" | ">" | ">=" | "=" | "-=" | "+=" | "*=" | "/=" | "%=" | "&=" 
          | "|=" | "^=" | "<<=" | ">>="
<const> ::= <int> | <long> | <uint> | <ulong> | <double> | <char>
<identifier> ::= ? An identifier token ?
<string> ::= ? A string token ?
<int> ::= ? An int token ?
<char> ::= ? A char token ?
<long> ::= ? An int or long token ?
<uint> ::= ? An unsigned int token ?
<ulong> ::= ? An unsigned int or unsigned long token ?
<double> ::= ? A floating-point constant token ?
```

## References

- Writing a C Compiler, _Nora Sandler_: https://nostarch.com/writing-c-compiler/
- C17 standard: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2310.pdf
<!---->
  
- System V x86-64 ABI: https://gitlab.com/x86-psABIs/x86-64-ABI/
- x86-64 instruction reference: https://www.felixcloutier.com/x86/
- Compiler explorer online: https://godbolt.org/

****

@romainducrocq

