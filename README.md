# [C++] Wheel C Compiler
> **WORK IN PROGRESS**

*__<ins>Reinventing the wheel</ins>__ (idiom): "Waste a great deal of time or effort in creating something that already exists."*  
Yet another C Compiler for Computers. It's a bit rough on the edges tho (standard-compliant with a subset of C17).  

****
_Resources_:
- Writing a C Compiler - Build a Real Programming Language from Scratch, Nora Sandler
- Migrated from Cython implementation after "_Development_ `13. Floating-point numbers`"
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
- [ ] 16\. Characters and strings  
- [ ] 17\. Supporting dynamic memory allocation  
- [ ] 18\. Structures  

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
    -E           preprocess with gcc, then compile

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

@romainducrocq
