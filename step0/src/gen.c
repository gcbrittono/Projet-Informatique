#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <gram.h>
#include <lex.h>
#include <rel.h>
#include <gen.h>
#include <global.h>
#include <notify.h>


unsigned int swap (unsigned int code){
	code = ((code >> 24)& 0x000000FF) | ((code << 24)& 
0xFF000000) | ((code >> 8)& 0x0000FF00) | ((code << 8)& 
0x00FF0000);
	return code;
}
