# [Lang] Coarse C Compiler
> **WORK IN PROGRESS**

Yet another C Compiler for Computers. It's a bit rough on the edges tho.

****
_Resources_:
- Writing a C Compiler - Build a Real Programming Language from Scratch, Nora Sandler
- Migrated from Cython implementation after "_Development_ `Floating-point numbers`"
****

## Development

### Language features

- [x] Integer constants  
- [x] Unary operators  
- [x] Binary operators  
- [x] Logical and relational operators  
- [x] Local variables  
- [x] Statements and conditional expressions  
- [x] Compound statements  
- [x] Loops  
- [x] Functions  
- [x] File-scope variables and storage-class specifiers  

### Types

- [x] Long integers  
- [x] Unsigned integers  
- [x] Floating-point numbers  
- [ ] Pointers  
- [ ] Arrays and pointer arithmetic  
- [ ] Characters and strings  
- [ ] Supporting dynamic memory allocation  
- [ ] Structures  

### Optimization

- [ ] Optimizing TAC programs  
- [ ] Register allocation  

## How To _

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
```

### Run
```
ccc2 path/to/file.c
```

### Help
```
Usage: ccc2 [Help] [Debug] [Link] [Lib] FILE

[Help]:
    --help       print help and exit

[Debug]:
    -v           enable verbose mode
    (Debug only):
    --lex        print  lexing    stage and exit
    --parse      print  parsing   stage and exit
    --validate   print  semantic  stage and exit
    --tacky      print  interm    stage and exit
    --codegen    print  assembly  stage and exit
    --codeemit   print  emission  stage and exit

[Link]:
    -S           compile, but do not assemble and link
    -c           compile and assemble, but do not link

[Lib]:
    -l<libname>  links with a library file

FILE:            .c file to compile
```

****

@romainducrocq
