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
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };
       
//identifier
enum { Token, Hash, Name, Class, Type, Val, HClass, HType, HVal, Idsz };


//types of variable
enum {CHAR, INT, PTR};


char *p, *lp,  //pointer to source code string;
     *data;   // data/bss pointer

int *e, *le,  // current position in emitted code
    *id,      // currently parsed identifier
    *sym,     // symbol table (simple list of identifiers)
    token,    // current token
    ival,     // current token value
    ty,       // current expression type
    loc,      // local variable offset
    line,     // current line number
    src,      // print source and assembly flag
    debug;    // print executed instructions

//Lexer
void next(){
	char *pp;	//last position
	int hash;
	 while (token = *p) {
	    ++p;
	    if (token == '\n') { //next line
	      /*if (src) {
	        printf("%d: %.*s", line, p - lp, lp);
	        lp = p;
	        while (le < e) {
	          printf("%8.4s", &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
	                           "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
	                           "OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[*++le * 5]);
	          if (*le <= ADJ) printf(" %d\n", *++le); else printf("\n");
	        }
	      }*/
	      ++line;
	    }
	    else if (token == '#') { //ignore #include
	    	while (*p != 0 && *p != '\n') ++p;
	    }
	    else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || token == '_') {
			pp = p - 1;
			hash = token;
	      	while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_')
	        	hash = hash * 147 + *p++;
	      	// look for existing identifier
	      	id = sym;
	      	while (id[Token]) { //id[0]
	        	if (hash == id[Hash] && !memcmp((char *)id[Name], pp, p - pp)) { 
					token = id[Token]; //found one, return
					return; 
				}
	        	id = id + Idsz;
	      	}
	      	// store new ID
	      	id[Name] = (int)pp;	
	      	id[Hash] = hash; 
	      	token = id[Token] = Id;
	      	return;
	    }
		else if (token >= '0' && token <= '9') {
	      	if (ival = token - '0') {  //dec
			  	while (*p >= '0' && *p <= '9') 
					ival = ival * 10 + *p++ - '0'; 
			}
		    else if (*p == 'x' || *p == 'X') { //hex
		    	while ((token = *++p) && ((token >= '0' && token <= '9') || (token >= 'a' && token <= 'f') || (token >= 'A' && token <= 'F')))
		        	ival = ival * 16 + (token & 15) + (token >= 'A' ? 9 : 0);
		    }
		    else {  //oct
				while (*p >= '0' && *p <= '7')
					ival = ival * 8 + *p++ - '0'; 
				}
		    token = Num;
		    return;
	    }
	    else if (token == '/') {	
	      	if (*p == '/') { //comment
	        	++p;
	        	while (*p != 0 && *p != '\n') 
					++p;
	      	}
	      	else {	//divide
	        	token = Div;
	        	return;
	      	}
	    }
	    else if (token == '\'' || token == '"') {
	      	pp = data;
	      	while (*p != 0 && *p != token) {
	        	if ((ival = *p++) == '\\'){
	          		if ((ival = *p++) == 'n')	ival = '\n';
	        	}
	        	if (token == '"')	*data++ = ival;
	      	}
	      	++p;
	      	if (token == '"')  
				ival = (int)pp; 
			else // if it is a single character, return Num token
				token = Num;
	      	return;
	    }
	    else if (token == '=') { if (*p == '=') { ++p; token = Eq; } else token = Assign; return; }
	    else if (token == '+') { if (*p == '+') { ++p; token = Inc; } else token = Add; return; }
	    else if (token == '-') { if (*p == '-') { ++p; token = Dec; } else token = Sub; return; }
	    else if (token == '!') { if (*p == '=') { ++p; token = Ne; } return; }
	    else if (token == '<') { if (*p == '=') { ++p; token = Le; } else if (*p == '<') { ++p; token = Shl; } else token = Lt; return; }
	    else if (token == '>') { if (*p == '=') { ++p; token = Ge; } else if (*p == '>') { ++p; token = Shr; } else token = Gt; return; }
	    else if (token == '|') { if (*p == '|') { ++p; token = Lor; } else token = Or; return; }
	    else if (token == '&') { if (*p == '&') { ++p; token = Lan; } else token = And; return; }
	    else if (token == '^') { token = Xor; return; }
	    else if (token == '%') { token = Mod; return; }
	    else if (token == '*') { token = Mul; return; }
	    else if (token == '[') { token = Brak; return; }
	    else if (token == '?') { token = Cond; return; }
	    else if (token == ';' || token == '{' || token == '}' || token == '(' || token == ')' || token == ']' || token == ',' || token == ':') return;
	}
}

void expr(int lev){
	
}

void stmt(){
	
}

int main(int argc, char **argv) {
	
	int bt, *idmain;
  	int *pc, *sp, *bp, a, cycle; //virtual machine registers
	int poolsize = 256 * 1024; //arbitrary size
	int i, *t; //i: how many characters
	int fd;//opening file
 
    if ((fd = open("MyCode.txt",0)) < 0) {
        printf("could not open text\n");
        return -1;
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
    i = Char; //i = 6
	while (i <= While){ // add keywords to symbol table, i <= 13
		next(); 
		id[Token] = i++;
	}
	i = OPEN; 
	while (i <= EXIT){ // add library to symbol table
		next(); 
		id[Class] = Sys; 
		id[Type] = INT;
		id[Val] = i++; 
	}
	next(); 
	id[Token] = Char; // handle void type
  	next(); 
	idmain = id; // keep track of main
    
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
    
    /*print token
    token = *p++;
    while (token > 0) {
        printf("token is: %c\n", token);
        token = *p++;
    }*/
    
    // parse declarations
  	line = 1;
  	next();
  	while(token){
  		bt = INT; // basetype
  		if (token == Int) next();
  		else if (token == Char) { 
			next(); 
			bt = CHAR; 
		}
		while (token != ';' && token != '}') {
			ty = bt;
      		while (token == Mul) { 
				next(); 
				ty = ty + PTR; 
				if (token != Id) { printf("%d: bad global declaration\n", line); return -1; }
			    if (id[Class]) { printf("%d: duplicate global definition\n", line); return -1; }
			    next();
			    id[Type] = ty;
			}
			if (token == '(') { // function
		        id[Class] = Fun;
		        id[Val] = (int)(e + 1);
		        next(); i = 0;
		        while (token != ')') {
		          ty = INT;
		          if (token == Int) next();
		          else if (token == Char) { next(); ty = CHAR; }
		          while (token == Mul) { next(); ty = ty + PTR; }
		          if (token != Id) { printf("%d: bad parameter declaration\n", line); return -1; }
		          if (id[Class] == Loc) { printf("%d: duplicate parameter definition\n", line); return -1; }
		          id[HClass] = id[Class]; id[Class] = Loc;
		          id[HType]  = id[Type];  id[Type] = ty;
		          id[HVal]   = id[Val];   id[Val] = i++;
		          next();
		          if (token == ',') next();
		        }
		        next();
		        if (token != '{') { printf("%d: bad function definition\n", line); return -1; }
		        loc = ++i;
		        next();
		        while (token == Int || token == Char) {
		          bt = (token == Int) ? INT : CHAR;
		          next();
		          while (token != ';') {
		            ty = bt;
		            while (token == Mul) { next(); ty = ty + PTR; }
		            if (token != Id) { printf("%d: bad local declaration\n", line); return -1; }
		            if (id[Class] == Loc) { printf("%d: duplicate local definition\n", line); return -1; }
		            id[HClass] = id[Class]; id[Class] = Loc;
		            id[HType]  = id[Type];  id[Type] = ty;
		            id[HVal]   = id[Val];   id[Val] = ++i;
		            next();
		            if (token == ',') next();
		          }
		          next();
		        }
		        *++e = ENT; *++e = i - loc;
		        while (token != '}') stmt();
		        *++e = LEV;
		        id = sym; // unwind symbol table locals
		        while (id[Token]) {
		          if (id[Class] == Loc) {
		            id[Class] = id[HClass];
		            id[Type] = id[HType];
		            id[Val] = id[HVal];
		          }
		          id = id + Idsz;
		        }
		    }
		    else {
		        id[Class] = Glo;
		        id[Val] = (int)data;
		        data = data + sizeof(int);
		    }
			if (token == ',') next();
    	}
    	next();
	}
	if (!(pc = (int *)idmain[Val])) { printf("main() not defined\n"); return -1; }
  	if (src) return 0;
    
    // setup stack
	bp = sp = (int *)((int)sp + poolsize);
	*--sp = EXIT; // call exit if main returns
	*--sp = PSH; t = sp;
	*--sp = argc;
	*--sp = (int)argv;
	*--sp = (int)t;
    
    return 0;
}
