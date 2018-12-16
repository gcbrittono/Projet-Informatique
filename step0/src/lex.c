
/**
 * @file lex.c
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexical analysis routines for MIPS assembly syntax.
 *
 * These routines perform the analysis of the lexeme of an assembly source code file.
 */

/*#define _POSIX_C_SOURCE 200112L*/
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>


/*fonction qui crée une liste*/
File creerFile(){
	return NULL;
}

/*fonction qui vérifie si la liste est vide*/
int estVide(File L){
	return !L;
}

/*fonction qui crée un élément de la lsite*/
lex_t creerElement(char* lex, etat cat, int lig){
	lex_t p;
	p.lexeme=strdup(lex);
	p.categorie=cat;
	p.ligne=lig;
	p.suiv=NULL;
	return p;
}

/*fonction qui ajoute un maillon à la fin de la file contenant le lexeme lex, de type cat et qui est sur la ligne lig, à la liste L*/
File enfiler(lex_t L, File F){
	File A=calloc(1,sizeof(*A));
	if (A==NULL)
		return NULL;
	*A=L;
	if (estVide(F))
		A->suiv=A;
	else{
		A->suiv=F->suiv;
		F->suiv=A;
	}
	return A;
}
	




/*afiche le premier élément de la file*/
void afficherFile(File F){
	if (estVide(F)){ /*cas ou la file est vide, on sort de la fonction*/
		printf("la file est vide \n");
		return;
	}
	char etats[20] 	;
	switch(F->suiv->categorie){
		case 0:
			strcpy(etats,"INIT");
			break;
		case 1:
			strcpy(etats,"COMMENTAIRE");
			break;
		case 2:
			strcpy(etats,"SYMBOLE");
			break;
		case 3:
			strcpy(etats,"DIRECTIVE");
			break;
		case 4:
			strcpy(etats,"REGISTRE");
			break;
		case 5:
			strcpy(etats,"DEUX_POINTS");
			break;
		case 6:
			strcpy(etats,"VIRGULE");
			break;
		case 7:
			strcpy(etats,"SAUT_DE_LIGNE");
			break;
		case 8:
			strcpy(etats,"HEXA_DEBUT");
			break;
		case 9:
			strcpy(etats,"OCTATE");
			break;
		case 10:
			strcpy(etats,"DECIMAL_ZEROS");
			break;
		case 11:
			strcpy(etats,"DECIMAL");
			break;
		case 12:
			strcpy(etats,"TERM");
			break;
		case 13:
			strcpy(etats,"HEXA");
			break;
		case 14:
			strcpy(etats,"CITATION");
			break;
		case 15:
			strcpy(etats,"PARENTHESE");
			break;
		case 17:
			strcpy(etats,"BASE_OF");
			break;
		default :
			WARNING_MSG("mauvais type d'opérande");
	}
	printf("Ligne %d : [ %s ] : %s \n",F->suiv->ligne, etats, F->suiv->lexeme);
}


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
    	if (current_line ==NULL) 
		return NULL;

    /* eats any blanks before the token*/
	while ((*start!='\0' && isblank(*start) )){
		start++;
    	}	

    /* go till end of a lexeme */ 
    	end=start; 

	switch(*start){
		case ('#'):
			while (*end!='\0')
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
				if (*end==':' || *end==',' || /**end=='(' || *end==')' || */*end=='"' || *end=='#'){
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

void lex_read_line( char *line, int nline, File* F, int* erreur) {
    	char* token = NULL;
    	char* current_address=line;


    /* TODO : faire l'analyse lexical de chaque token ici et les ajouter dans une collection*/
    /* ATTENTION: getNextToken est à recoder completement*/
    	while( (current_address= getNextToken(&token, current_address)) != NULL){
    
  	/* definition des etats*/
	/*enum {INIT, COMMENTAIRE, SYMBOLE, DIRECTIVE, REGISTRE,  DEUX_POINTS, VIRGULE, SAUT_DE_LIGNE, HEXA_DEBUT, OCTATE, DECIMAL_ZEROS, DECIMAL, TERM, HEXA, CITATION};*/
  /*mise en oeuvre de lautomate*/

  	etat S = INIT; /*Definition detat de lautomate*/
  	char* c=token; /*caractere analyse courant*/
 
	while (*c!='\0'){
  		switch(S) {
  			case INIT:
				if(isdigit(*c)){/*il separe les chiffres des caracteres*/
					S = (*c=='0')?DECIMAL_ZEROS : DECIMAL;	
				}
				else if (isspace(*c)) S=INIT;
				else if(isalpha(*c)){
					S = SYMBOLE;
				}
				else if (iscntrl(*c)){
					if (*c == '\n') S = SAUT_DE_LIGNE;
					else{
						WARNING_MSG("Erreur INIT ligne %d\n",nline);
						*erreur =1;
					}
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
							S = PARENTHESE;
							break;
						case ')':
							S = PARENTHESE;
							break;
						case '"':
							S = CITATION;
							break;
						case '-':
							S = SIGNE;
							break;
						case '+':
							S = SIGNE;
							break;
						default:
						 	WARNING_MSG("Erreur INIT ligne %d\n",nline);
							*erreur =1;
					}
				}
				break;
		
			case COMMENTAIRE:
				break;
		
			case SYMBOLE:
				if(*c == '(') S = BASE;
				else if (isalnum(*c)) S = SYMBOLE;
				else if (*c == ':'||*c == '.') S = TERM;
				else if (isspace(*c)) S = TERM;
				else{
					WARNING_MSG("Erreur SYMBOLE ligne %d\n",nline);
					*erreur =1;
				}
				break;
		
			case DIRECTIVE:
				if (isalpha(*c)) S = DIRECTIVE;
				else if(isspace(*c)) S = TERM;
				else{
					WARNING_MSG("Erreur DIRECTIVE ligne %d\n",nline);
					*erreur =1;
				}
				break;
		
			case REGISTRE:
				if (isalnum(*c)) S = REGISTRE;
				else if (*c == ',') S = TERM;
				else if (isspace(*c)) S = TERM;
				else{
					WARNING_MSG("Erreur REGISTRE ligne %d\n",nline);
					*erreur =1;
				}
				break;
		
			case DEUX_POINTS:
				if (isspace(*c)) S = TERM;
				else{
					WARNING_MSG("Erreur DEUX-POINTS ligne %d\n",nline);
					*erreur =1;
				}
				break;
		
			case PARENTHESE:
				if (isspace(*c)) S = TERM;
				else{
					WARNING_MSG("Erreur PARENTHESE ligne %d\n",nline);
					*erreur =1;
				}
				break;

			case VIRGULE:
				if (isspace(*c)) S = TERM;
				else{
					WARNING_MSG("Erreur VIRGULE ligne %d\n",nline);
					*erreur =1;
				}
				break;
		
			case SAUT_DE_LIGNE:
				S = TERM;
				break;

			case SIGNE:
				if(isdigit(*c))
					if(*c=='0')S=DECIMAL_ZEROS;
					else S=DECIMAL;	
				else{
					WARNING_MSG("Erreur SIGNE ligne %d\n",nline);
					*erreur =1;
				}
				break;
		
			case DECIMAL_ZEROS:
				if(*c == 'x'||*c == 'X' ) S = HEXA_DEBUT;
				else if (*c == '\n') S = TERM;
				else if (isspace(*c)) S = TERM;
				else if (isdigit(*c)&& 0<=(*c)<=7)S = OCTATE;
				else{
					WARNING_MSG("Erreur DECIMAL-ZEROS ligne %d\n",nline);
					*erreur =1;
				}
				break;

			case HEXA_DEBUT:
				if(isxdigit(*c)) S = HEXA;
				else{
					WARNING_MSG("Erreur HEXA_DEBUT ligne %d\n",nline);
					*erreur =1;
				}
				break;

			case OCTATE:
				if (isdigit(*c)&&(0<=(*c)<=7))S = OCTATE;
				else if(isspace(*c) && *c == '\n') S = TERM;
				else if(*c == '(') S = BASE;
				else{
					WARNING_MSG("Erreur OCTATE ligne %d\n",nline);
					*erreur =1;
				}
				break;

			case DECIMAL:
				if(isdigit(*c)) S = DECIMAL;
				else if(isspace(*c) && *c == '\n') S = TERM;
				else if(*c == '(') S = BASE;
				else{
					WARNING_MSG("Erreur DECIMAL ligne %d\n",nline);
					*erreur =1;
				}
				break;

			case HEXA:
				if(isxdigit(*c)) S = HEXA;
				else if(isspace(*c) && *c == '\n') S = TERM;
				else if(*c == '(') S = BASE;
				else{
					WARNING_MSG("Erreur HEXA ligne %d",nline);
					*erreur =1;
				}
				break;
			case TERM:
				break;	

			case CITATION:
				break;
			case BASE:
				if( *c == ')') S = BASE_OF;
				else if (*(c+1)=='\0'){
					WARNING_MSG(" ok erreur base offset ligne %d",nline);/*erreur si pas de parenthèse a la fin*/
					*erreur =1;
				}
				break;
			case BASE_OF:
				break;
  		}
		c++;
		
 	} 
	if (S == BASE){
		WARNING_MSG("erreur base offset ligne %d",nline);
		*erreur =1;
	}
	*F=enfiler(creerElement(token, S ,nline),*F);
	S=INIT;
    }
}


/**
 * @param file Assembly source code file name.
 * @param nlines Pointer to the number of lines in the file.
 * @return should return the collection of lexemes
 * @brief This function loads an assembly code from a file into memory.
 *
 */
void lex_load_file( char *file, unsigned int *nlines, File* F, int* erreur) {
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
            		if ( 0 != strlen(line) ) {
                		lex_read_line(line,*nlines,F, erreur);
            		}
        	}
	
    	}
	
/*fermeture du fichier et fin de fonction*/		
    	fclose(fp);
    	return;
}



