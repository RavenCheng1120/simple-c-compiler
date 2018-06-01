#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

//tokens and classes
enum {
  Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak};

//instructions(It is in order!!)
enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT };
       
//identifier
enum {Token, Hash, Name, Type, Class, Value, BType, BClass, BValue, IdSize};

//types of variable
enum {CHAR, INT, PTR};


char *p, *lp,  //pointer to source code string;
     *data;   // data/bss pointer

int *e, *le,  // current position in emitted code
    *id,      // currently parsed identifier
    *sym,     // symbol table (simple list of identifiers)
    token,    //current token
    ival,     // current token value
    ty,       // current expression type
    loc,      // local variable offset
    line,     // current line number
    src,      // print source and assembly flag
    debug;    // print executed instructions

void next(){
	
}

void expr(int lev){
	
}

void stmt(){
	
}

int main(int argc, char **argv) {
	
	int bt, ty, *idmain;
  	int *pc, *sp, *bp, a, cycle; //virtual machine registers
	int poolsize = 256 * 1024; //arbitrary size
	int i, *t; //i: how many characters
	int fd;//opening file
 
    if ((fd = open("MyCode.txt",0)) < 0) {
        printf("could not open text\n");
        return -1;
    }
    if (!(p = lp = malloc(poolsize))) {
        printf("could not malloc(%d) for source area\n", poolsize);
        return -1;
    }
 
    //read the source file "MyCode.txt"
    if ((i = read(fd, p, poolsize-1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }
    p[i] = 0; //add EOF character
    close(fd);
    
    //print token
    token = *p++;
    while (token > 0) {
        printf("token is: %c\n", token);
        token = *p++;
    }
    
    //allocate memory for virtual machine
    if (!(e = le = malloc(poolsize))) {
        printf("could not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if (!(sp = malloc(poolsize))) {
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }
    if (!(sym = malloc(poolsize))) { 
		printf("could not malloc(%d) symbol area\n", poolsize); 
		return -1; 
	}
 	
 	//copy poolsize number of 0s to the three segments
    memset(sym,  0, poolsize);
  	memset(e,    0, poolsize);
  	memset(data, 0, poolsize);
    
    p = "char else enum if int return sizeof while "
      "open read close printf malloc free memset memcmp exit void main";
    i = Char; 
	 
    
    
    return 0;
}
