#ifndef _GEN_H_
#define _GEN_H_


#include <stdio.h>
#include <lex.h>
#include <gram.h>

struct R{
	unsigned int func:6,
	sa:5,
	rd:5,          
	rt:5,         
	rs:5,    
	opcode:6;   
};           


#endif 
