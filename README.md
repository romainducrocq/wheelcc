# wheelcc - the c compiler reinventing the wheel
> **THIS COMPILER IS A WORK IN PROGRESS! ANYTHING CAN CHANGE AT ANY MOMENT WITHOUT ANY NOTICE! USE THIS COMPILER AT YOUR OWN RISK!**

*__<ins>Reinventing the wheel</ins>__ (idiom): "Waste a great deal of time or effort in creating something that already exists."*
<!---->
A small, self-contained C compiler written from scratch in C++ for x86-64 GNU/Linux platforms. 

## Overview

The wheelcc compiler supports a large subset of C17 ([International Standard ISO/IEC 9899:2018](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2310.pdf)), for which it has it's own built-in preprocessor, frontend, IR and backend. It emits x86-64 AT&T assembly for GNU/Linux, which is then linked with gcc/ld. The project is entirely written in C++17, and builds to a standalone executable plus a driver in bash. wheelcc is overall designed after Nora Sandler's [Writing a C Compiler](https://nostarch.com/writing-c-compiler), and was tested against it's [test suite](https://github.com/nlsandler/writing-a-c-compiler-tests).

## Usage

### Install

Get the repo, cd to the bin directory  
```
$ git clone --depth 1 --branch master https://github.com/romainducrocq/wheelcc.git
$ cd wheelcc/bin/
```
Configure the repo and install the build/runtime dependencies: `gcc g++ make cmake`  
```
$ ./configure.sh
```
Build the compiler in Release mode  
```
$ ./make.sh
```
Install the `wheelcc` command system-wide (creates a symlink to the driver in `/usr/local/bin/`). As an alternative, do not install and use `wheelcc/bin/driver.sh` instead  
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

Compile and run  
```
$ wheelcc main.c
$ ./main
Hello, World!
```

Usage
> **Note**: Except for one source file to compile, all other command-line arguments are optional. 
> However,  <ins>the order of arguments passed matters</ins>: they are parsed only in this order, any other order will fail!
```
$ wheelcc --help
Usage: wheelcc [Help] [Debug] [Preprocess] [Include] [Link] [Linkdir] [Linklib] [Output] FILES

[Help]:
    --help          print help and exit

[Debug]:
    -v              enable verbose mode
    (Test/Debug build only):
    --lex           print  lexing    stage and exit
    --parse         print  parsing   stage and exit
    --validate      print  semantic  stage and exit
    --tacky         print  interm    stage and exit
    --codegen       print  assembly  stage and exit
    --codeemit      print  emission  stage and exit

[Preprocess]:
    -E              enable macro expansion with gcc

[Include]:
    -I<includedir>  add a list of paths to include path

[Link]:
    -S              compile, but do not assemble and link
    -c              compile and assemble, but do not link

[Linkdir]:
    -L<linkdir>     add a list of paths to link path

[Linklib]:
    -l<libname>     link with a list of library files

[Output]:
    -o <file>       write the output into <file>

FILES:              list of .c files to compile
```

### Test

cd to the test directory, get the testtime dependencies: `diffutils valgrind`
```
$ cd wheelcc/test/
$ ./get-dependencies.sh
```

Test the compiler
```
$ ./test-compiler.sh
```

Test the preprocessor
```
$ ./test-preprocessor.sh
```

Test memory leaks
```
$ ./test-memory.sh
```

wheelcc is frequently tested on these distributions (x86-64)
Debian GNU/Linux  | Ubuntu           | Rocky Linux      | Arch Linux
:---:             |:---:             |:---:             |:---:
:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:

## Development

### Language features

- [x] 1\. Integer constants
- [x] 2\. Unary operators
- [x] 3\. Binary operators
- [x] 4\. Logical and relational operators
- [x] 5\. Local variables
- [x] 6\. Statements and conditional expressions
- [x] 7\. Compound statements
- [x] 8\. Loops
- [x] 9\. Functions
- [x] 10\. File-scope variables and storage-class specifiers

### Types

- [x] 11\. Long integers
- [x] 12\. Unsigned integers
- [x] 13\. Floating-point numbers
- [x] 14\. Pointers
- [x] 15\. Arrays and pointer arithmetic
- [x] 16\. Characters and strings
- [x] 17\. Supporting dynamic memory allocation
- [x] 18\. Structures

### Optimization

- [ ] 19\. Optimizing TAC programs
- [ ] 20\. Register allocation

****

@romainducrocq
