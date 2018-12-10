#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <gram.h>
#include <lex.h>
#include <rel.h>

void registre( char* reg, registres tab[32], int ligne){
	int i=0;;
	int ok=0;
	int indice=-1;
	while (i<32 ){
		if (strcmp(reg,tab[i].mnemo)==0)
			indice=i;
		else if(strcmp(reg,tab[i].reg)==0)
			ok=1;
		i+=1;
	}
	if(indice!=-1)
		strcpy(reg,tab[indice].reg);
	else if(ok==0)
		printf("erreur ce registre n'existe pas ligne %d \n",ligne);/*gestion erreurs arreter compil?*/
}

ListeG inserer(void* e, ListeG L){
	ListeG A=calloc(1,sizeof(A));
	if (A==NULL)
		return NULL;
	(A->pval)=e;
	if (listeVide(L)){
		A->suiv=A;
		return A;
	}
	else{
		A->suiv=L->suiv;
		L->suiv=A;
	}
	return L;
}

void associerReg(ListeG Inst,registres tableau[32],int ligne){
	int nb=((Instruction*)(Inst->pval))->nbop;
	int i = 0;
	for(i; i<nb;i++){
		if(((Instruction*)(Inst->pval))->op[i].categorie==REGISTRE)
			registre(((Instruction*)(Inst->pval))->op[i].lexeme,tableau,ligne);
	}
}

void chargeRegistre(registres tab[32]){
FILE* re;

	re = fopen("src/registre.txt","r");
	if (re == NULL){
		printf("le dictionnaire de registre n'a pas été ouvert \n"); 
		return; /*gestion erreurs*/
	}
	char* reg1=malloc(sizeof(*reg1));
	char* reg2=malloc(sizeof(*reg2));
	int i;
	for (i=0;i<32;i++){
        	fscanf(re, "%s %s",reg1,reg2);
		tab[i].mnemo=strdup(reg1);
		tab[i].reg=strdup(reg2);
	}
	fclose(re);
}

void rel(ListeG Instruct){
	registres tab[32];
	chargeRegistre(tab);
	if(listeVide(Instruct))
		printf("la section TEXT est vide");
	else{
		ListeG A=Instruct;
		do{
			associerReg(A->suiv,tab,((Instruction*)(A->suiv->pval))->ligne);






			A=A->suiv;
		}while (A!=Instruct);
	}
}







/*typedef struct {
	char* nom;
	etat type;
	char type_inst;
	int nbop;
	int ligne;
	unsigned int decalage;
	Operande op[3];
} Instruction;*/



/**********fONCTION UTILISÉ POUR COMPTER LE NOMBRE DE BITS*******************************************************************************/
int nombre_bits_variable(int operande)
{
    int i = operande;
    int compteur = 0;
    while(i > 0)
    {
        i >>= 1;
        compteur++;
    }
    return compteur;
}


/******************************************Fonction pour separer les operandes de type offset(base)******************************************************************/

char* get_operandes_offsetbase(char* offset, char* base){

    char* start = NULL;
    char* end = NULL;
    int count_x = 1;

	while ((*start!='\0' && isblank(*start) )){
		start++;
    	}

        end = start;	


    switch(*start){

        case(isdigit(*start)||isxdigit(*start)):
            if ((*start='x'||*start='X') && count_x = 1){
                count_x = 0;  
                *offset = *start;
                offset++;          
                end++;
            }
            else if ((*start='x'||*start='X') && count_x = 0)
                printf("Format d'offset invalid, plus d'un character x dans la definition hexadecimal");
            
            else
                end++;
        
        break;

        case(isalnum(*start)):
            
            if (count_x = 1 && *start = '$'){
                count_x = 0;
                *base = *start;
                 base++;
                end++;
            }
            else if (count_x = 0 && *start = '$')
                printf("Format de base invalid, plus d'un character $");
            
            else 
                end++; 

        break;

        default:
            if (*end = '(' || *end = ')'){
                token_size = end-start;
                if (token_size > 0){
                    *token = calloc(token_size+1,sizeof(*start));
                    strncpy(*token,start,token_size);
                    (*token)[token_size] = '\0';
                    return end;
                } 
            }
            end++;            

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


/****************************************************************************************************************************************/

/*typedef enum {INIT, COMMENTAIRE, SYMBOLE, DIRECTIVE, REGISTRE,  DEUX_POINTS, VIRGULE, SAUT_DE_LIGNE, HEXA_DEBUT, OCTATE, DECIMAL_ZEROS, DECIMAL, TERM, HEXA, CITATION,PARENTHESE} etat;*/

/****************************************************************/
/******** Machine a etats pour la verification des operandes*****/
/****************************************************************/

int extraction_des_operandes(Instruction* inst, int pos_operand, Dico hashTable[60]){
	enum {INIT_OP, NOMBRE_ENTIER, REGISTRE_OP, OFFSET_BASE, SYMBOLE_OP};

    char* offset_bas;
    char* base_bas;
    

	char* instr_nom;	
	char* dummy_op;
	int state = INIT;
	int index;
	int nombreInstruc;

	FILE* dictionnaire;

	dictionnaire = fopen("src/dictionnaire_instruction.txt","r");
	if (dictionnaire == NULL){
		printf("le dictionnaire n'a pas été ouvert \n"); 
		return; /*gestion erreurs*/
	}

	fscanf(dictionnaire, "%d",&nombreInstruc); 



	etat inst_categorie;
	type_operande typeofoperande;

	intsr_nom = inst->nom;
	dummy_op = inst->op[pos_operand]->lexeme;
	inst_categorie = inst->op[pos_operande]->categorie;
	typeofoperande = inst->op[pos_operande]->type;
	
	index = funHash(instr_nom,nombreInstruc);

	switch (state)
​	{
	    case INIT_OP:
		if(ints_categorie = SYMBOLE)
			state = SYMBOLE_OP;
		else if (ints_categorie = BASE_OF)
			state = OFFSET_BASE;
		else if(ints_categorie = DECIMAL)
			state = NOMBRE_ENTIER;
		else if(ints_categorie = REGISTRE)
			state = REGISTRE_OP;
	        break;

/*****************************************************************************************************************************************/
/***************************************************************case nombre entier********************************************************/
/*****************************************************************************************************************************************/

	    case NOMBRE_ENTIER:
        if(strcmp(hashTable[index].type_operande[pos_operande],"imm")==0)/******************** entier signe Immediat***************************/
        {			
           if (atoi(lexeme)>SHRT_MIN && atoi(lexeme)<SHRT_MAX)			
				return 1;
			else 
				return 0;	
		};	
/*****************************************************************************************************************************************/		

		else if(strcmp(hashTable[index].type_operande[pos_operande],"sa")==0)/*****************************entier non signe sa*****************************/
		{	
			if (atoi(lexeme)>0 && atoi(lexeme)<31)
				return 1;		
			else 
				return 0;
		};

	        break;
/****************************************************************************************************************************************/		
		/*J'ai du mal a faire la liaison entre les nombre des bits et l'operande****/
		else if(strcmp(hashTable[index].type_operande[pos_operande],"rel")==0)/*****************************Relatif rel ***********************************/
		{	
			if (atoi(lexeme)%4 == 0)
				return 1;		
			else 
				return 0;
		};

	        break;
/*****************************************************************************************************************************************/

		/*J'ai du mal a faire la liaison entre les nombre des bits et l'operande****/
		else if(strcmp(hashTable[index].type_operande[pos_operande],"abs")==0)/*****************************Relatif abs *****************************/
		{	
			if (atoi(lexeme)%4 == 0)
				return 1;		
			else 
				return 0;
		};

	        break;





	    case REGISTRE_OP:
		if(strcmp(hashTable[index].type_operande[pos_operande],"reg")==0) /*registre  reg*/
		{
			if (atoi(lexeme)>0 && atoi(lexeme)<31)
				return 1;
			else 
				return 0;


		}; 


	        break;





	    case OFFSET_BASE:


        while( (current_address= get_operandes_offsetbase(offset_bas, base_bas)) != NULL){
            char* c=token;

        }


while (*c!='\0'){
        if (*c=='('){
            s = OFFSET
        else if(*c == ')')
            s = BASE

        }
		c++;
 	} 


/*****************************************************************************************************************************************/




		if(strcmp(hashTable[index].type_operande[pos_operande],"bas")==0) /*registre*/ 
		{	
			if (atoi(base_bas)>0 && atoi(base_bas)<31)
	        	{
				if (atoi(offset_bas)>SHRT_MIN && atoi(offset_bas)<SHRT_MAX)/* entier signe Immediat*/
					return 1;
			}

		

		else
			return 0;
		
		};

		break;

	    case SYMBOLE_OP:

	        break;


	    default:

	}


}
















