# [C++] Wheel C Compiler
> **THIS COMPILER IS A WORK IN PROGRESS!**  
> **ANYTHING CAN CHANGE AT ANY MOMENT WITHOUT ANY NOTICE! USE AT YOUR OWN RISK!**  

*__<ins>Reinventing the wheel</ins>__ (idiom): "Waste a great deal of time or effort in creating something that already exists."*  
Yet another C Compiler for Computers. It's a bit rough on the edges (standard-compliant with a large subset of C17).  
<!---->

- Writing a C Compiler - Build a Real Programming Language from Scratch : https://norasandler.com/book/
- An Incremental Approach to Compiler Construction : http://scheme2006.cs.uchicago.edu/11-ghuloum.pdf
- C17 International Standard ISO/IEC 9899:2018 : https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2310.pdf
****

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

## How To _

### Get
```
git clone --depth 1 --branch master https://github.com/romainducrocq/LANG-WheelCC.git
cd LANG-WheelCC/
```

### Build
```
cd bin/
./configure.sh
./make.sh
./install.sh
. ~/.bashrc
```

### Test
```
cd test/
./test.sh
./valgrind.sh
```

### Run
```
wheelcc path/to/file.c
```

### Help
```
Usage: wheelcc [Help] [Dbg] [Pre] [Link] [Lib] [Out] FILES

[Help]:
    --help       print help and exit

[Dbg]:
    -v           enable verbose mode
    (Debug only):
    --lex        print  lexing    stage and exit
    --parse      print  parsing   stage and exit
    --validate   print  semantic  stage and exit
    --tacky      print  interm    stage and exit
    --codegen    print  assembly  stage and exit
    --codeemit   print  emission  stage and exit

[Pre]:
    -E           do not preprocess, then compile

[Link]:
    -S           compile, but do not assemble and link
    -c           compile and assemble, but do not link

[Lib]:
    -l<libname>  link with a list of library files

[Out]:
    -o <file>    write the output into <file>

FILES:           list of .c files to compile
```

****

This compiler was developped and tested with:  
```
gcc      (Debian 10.2.1-6) 10.2.1 20210110
g++      (Debian 10.2.1-6) 10.2.1 20210110
GLIBC    2.31
GLIBCXX  20210110

OS       Debian GNU/Linux 11 (bullseye) on Windows 10 x86_64
Kernel   5.15.133.1-microsoft-standard-WSL2
Shell    bash 5.1.4
CPU      11th Gen Intel i7-11850H (16) @ 2.496GHz
Memory   15860MiB
```

****

@romainducrocq
