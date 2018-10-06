
/**
 * @file lex.c
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexical analysis routines for MIPS assembly syntax.
 *
 * These routines perform the analysis of the lexeme of an assembly source code file.
 */

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>



/* ATTENTION: getNextToken est juste un exemple -> il est à recoder completement !!! */
/**
 * @param token The pointeur to the lexeme to be extracted.
 * @param current_line The address from which the analysis must be performed.
 * @return the address at which the analysis stopped or NULL if the analysis is terminated.  
 * @brief This function get an input line, extracts a token from it and return the pointeur to the next place where the analysis should continue. 
 */

/* note that MIPS assembly supports distinctions between lower and upper case*/
char* getNextToken(char** token, char* current_line) {
    char* start = current_line; 
    char* end=NULL; 
    int token_size=0;

    /* check input parameter*/ 
    	if (current_line ==NULL) return NULL;

    /* eats any blanks before the token*/
	while ((*start!='\0' && isblank(*start) )){
		start++;
    }	

    /* go till end of a lexeme */ 
    end=start; 

	switch(*start){
		case ('#'):
			while (*end!='\n')
				end++;
			while (*end!='\0' && !isblank(*end))
				end++;
			break; /*eats any blanks or end of words or enter at the end of a comment*/
	
		case (':') :
			end++;
			break;
		case (',') :
			end++;
			break;
		case ('(') :
			end++;
			break;
		case (')') :
			end++;
			break;
		case ('\n') :
			end++;
			break;
		case ('"'):
			end++;
			while (*end!='"'){
				if ((*end=='\\') && (*(end++)=='"')){
					end++;
				}
				end++;
			}
			end++;
			break;

		default:
			while (*end!='\0' && !isblank(*end)){	
				if (*end==':' || *end==',' || *end=='(' || *end==')' || *end=='"' || *end=='#'){
					token_size=end-start;
    					if (token_size>0){
						*token 	= calloc(token_size+1,sizeof(*start));
						strncpy(*token,start,token_size);
						(*token)[token_size]='\0';       
						return end;
					}
				}
				end++;
			}
	}


    /*compute size : if zero there is no more token to extract*/ 	
    token_size=end-start;
    if (token_size>0){
	*token 	= calloc(token_size+1,sizeof(*start));
	strncpy(*token,start,token_size);
	(*token)[token_size]='\0';       
	return end;
    } 
    return NULL;
}




/**
 * @param line String of the line of source code to be analysed.
 * @param nline the line number in the source code.
 * @return should return the collection of lexemes that represent the input line of source code.
 * @brief This function performs lexical analysis of one standardized line.
 *
 */

void lex_read_line( char *line, int nline) {
    char* token = NULL;
    char* current_address=line;


    /* TODO : faire l'analyse lexical de chaque token ici et les ajouter dans une collection*/
    /* ATTENTION: getNextToken est à recoder completement*/
    while( (current_address= getNextToken(&token, current_address)) != NULL){
    
  	/* definition des etats*/
	enum {INIT, COMMENTAIRE, SYMBOLE, DIRECTIVE, REGISTRE,  DEUX_POINTS, VIRGULE, SAUT_DE_LIGNE, HEXA_DEBUT, OCTATE, DECIMAL_ZEROS, DECIMAL, TERM, HEXA, CITATION};
  /*mise en oeuvre de lautomate*/

  int S = INIT; /*Definition detat de lautomate*/
  char* c=token; /*caractere analyse courant*/
 
while (*c!='\0'){
  switch(S) 
  {
  	case INIT:
	
		/*if(isascii(c)){*/
			if(isdigit(*c)){/*il separe les chiffres des caracteres*/
				S = (*c=='0')?DECIMAL_ZEROS : DECIMAL;	
			}
			else if (isspace(*c)) S=INIT;
			else if(isalpha(*c)){
				S = SYMBOLE;
			}
			else if (iscntrl(*c)){
				if (*c == '\n') S = SAUT_DE_LIGNE;
				else  printf ("Erreur1 ");
			}
			else{
				switch(*c){
					case '#':
						S = COMMENTAIRE;
						break;
					case '_':
						S = SYMBOLE;
						break;
					case '.':
						S = DIRECTIVE;
						break;
					case '$':
						S = REGISTRE;
						break;
					case ':':
						S = DEUX_POINTS;
						break;
					case ',':
						S = VIRGULE;
						break;
					case '(':
						S = SYMBOLE;
						break;
					case ')':
						S = SYMBOLE;
						break;
					case '"':
						S = CITATION;
						break;
					case '-':
						S = DECIMAL;
						break;
					default:
						 printf ("Erreur2 ");
				}
			}
		/*}*/
		/*else printf ("Erreur ");*/
		break;
		
	case COMMENTAIRE:
		break;
		
	case SYMBOLE:
		if (isalpha(*c)) S = SYMBOLE;
		else if (*c == ':'||*c == '.') S = TERM;
		else if (isspace(*c)) S = TERM;
		else printf ("Erreur 3");
		break;
		
	case DIRECTIVE:
		if (isalpha(*c)) S = DIRECTIVE;
		else if(isspace(*c)) S = TERM;
		else printf ("Erreur 4");
		break;
		
	case REGISTRE:
		if (isalnum(*c)) S = REGISTRE;
		else if (*c == ',') S = TERM;
		else if (isspace(*c)) S = TERM;
		else printf ("Erreur 5");
		break;
		
	case DEUX_POINTS:
		if (isspace(*c)) S = TERM;
		else printf ("Erreur 6");
		break;
		
	case VIRGULE:
		if (isspace(*c)) S = TERM;
		else printf ("Erreur7");
		break;
		
	case SAUT_DE_LIGNE:
		S = TERM;
		break;
		
	case DECIMAL_ZEROS:
		if(*c == 'x') S = HEXA_DEBUT;
		else if (*c == '\n') S = TERM;
		else if (isspace(*c)) S = TERM;
		if (isdigit(*c)||((0<=*c)<=7))S = OCTATE;
		else printf("Erreur 8");
		break;

	case HEXA_DEBUT:
		if(isxdigit(*c)) S = HEXA;
		else printf("Erreur9");
		break;

	case OCTATE:
		if (isdigit(*c)||((0<=*c)<=7))S = OCTATE;
		else if(isspace(*c) && *c == '\n') S = TERM;
		else printf("Erreur10");
		break;

	case DECIMAL:
		if(isdigit(*c)) S = DECIMAL;
		else if(isspace(*c) && *c == '\n') S = TERM;
		else printf("Erreur11");
		break;

	case HEXA:
		if(isxdigit(*c)) S = HEXA;
		else if(isspace(*c) && *c == '\n') S = TERM;
		else printf("Erreur12");
		break;
	case TERM:
		break;	

	case CITATION:
		break;
		
	
  
  
  }
c++;
 } 
  
  
 printf("%d : ",S); 
        puts(token);
    }
	
    return;
}
/*void lex_read_line( char *line, int nline) {
    char* token = NULL;
    char* current_address=line;*/


    /* TODO : faire l'analyse lexical de chaque token ici et les ajouter dans une collection*/
    /* ATTENTION: getNextToken est à recoder completement*/
/*    while( (current_address= getNextToken(&token, current_address)) != NULL){ 

        puts(token);
    }

    return;
}*/

/**
 * @param file Assembly source code file name.
 * @param nlines Pointer to the number of lines in the file.
 * @return should return the collection of lexemes
 * @brief This function loads an assembly code from a file into memory.
 *
 */
void lex_load_file( char *file, unsigned int *nlines ) {

    FILE        *fp   = NULL;
    char         line[STRLEN]; /* original source line */



    fp = fopen( file, "r" );
    if ( NULL == fp ) {
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        ERROR_MSG("Error while trying to open %s file --- Aborts",file);
    }

    *nlines = 0;

    while(!feof(fp)) {

        /*read source code line-by-line */
        if ( NULL != fgets( line, STRLEN-1, fp ) ) {
            line[strlen(line)-1] = '\0';  /* eat final '\n' */
            (*nlines)++;
		/*printf("%d \n",*nlines);*/

            if ( 0 != strlen(line) ) {
                lex_read_line(line,*nlines);
            }
        }
    }

    fclose(fp);
    return;
}



