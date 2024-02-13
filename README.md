# [Lang] Coarse C Compiler 

****

- Writing a C Compiler - Build a Real Programming Language from Scratch, Nora Sandler: https://nostarch.com/writing-c-compiler  
    - Test suite: https://github.com/nlsandler/writing-a-c-compiler-tests.git  
    - OCaml reference implementation: https://github.com/nlsandler/nqcc2  
- Migrated from Cython implementation after `[13] floating-point numbers`: https://github.com/romainducrocq/LANG-CCC/

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
    -c           compile, but do not link

[Lib]:
    -l<libname>  links with a library file

FILE:            .c file to compile
```

****

@romainducrocq
