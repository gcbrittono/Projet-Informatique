#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>
#include "gram.h"
#include "lex.h"
#include <unistd.h>


/*fonction qui crée une liste*/
ListeG creerListeG(){
	return NULL;
}

/*fonction qui vérifie si la liste est vide*/
int listeVide(ListeG L){
	return !L;
}


Instruction creerInstruction(char* lex, etat cat,int nombop, int lig,unsigned int dec /*,ListeG operande*/){
	Instruction p;
	p.nom=strdup(lex);
	p.type=cat;
	p.nbop=nombop;
	p.ligne=lig;
	p.decalage=dec;
	/*p.op=operande;*/
	return p;
}

Donnee1 creerDonnee1(char* lex, etat cat, int nombop, int lig, int dec, ListeG operande){
	Donnee1 p;
	p.lexeme=strdup(lex);
	p.type=cat;
	p.nbop=nombop;
	p.ligne=lig;
	p.decalage=dec;
	p.op=operande;
	return p;
}

Donnee2 creerDonnee2(char* lex, etat cat, int nombop, int lig, int dec, int val){
	Donnee2 p;
	p.lexeme=strdup(lex);
	p.type=cat;
	p.nbop=nombop;
	p.ligne=lig;
	p.decalage=dec;
	p.valeur=val;
	return p;
}

Symbole creerSymbole(char* lex,	etat cat, int lig, Section section ,int dec){
	Symbole p;
	p.lexeme=strdup(lex);
	p.type=cat;
	p.ligne=lig;
	p.decalage=dec;
	p.sect=section;
	return p;
}

ListeG ajouterQueue(void* e, ListeG L){
	ListeG A=calloc(1,sizeof(*A));
	if (A==NULL)
		return NULL;
	(A->pval)=e;
	if (listeVide(L))
		A->suiv=A;
	else{
		A->suiv=L->suiv;
		L->suiv=A;
	}
	return A;
}


/*lex_t defiler(File F){
	if (estVide(F))
		return NULL;
	File A=F->suiv;
	F->suiv=F->suiv->suiv;
	lex_t t=*A;
	free(A);
	return t;
}*/
/*Fonction qui a comment entrée la chaine des characters et comment sortie le code hash liée a cette entrée*/
int funHash(char* str, int taille){ /* Trouver en https://stackoverflow.com/questions/7666509/hash-function-for-string*/
	char new[32];
	snprintf(new, 32, "%s", str);
	toLowerStr(new);
	
	int hash, i;
	int len = strlen(new);
	
	
    for(hash = i = 0; i < len; ++i)
    {
        hash += new[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    
    return (int)fabs(hash % taille);
}


/*Fonction utilisée dans la fonction funHash pour convertir les character string en son equivalent minuscule*/
void toLowerStr(char *str){
	int lenght = strlen(str);
	int i;
	for(i = 0; i < lenght; i++){
		str[i] = tolower(str[i]);
	}
}



void machine_a_etat_gram (File F, ListeG Inst, ListeG Symb, ListeG Do1, ListeG Do2, Dico tableau[], int taille){
    /*definition des etats et des décalages dans chaque section*/
	Section Sect;
	int dec_text=0;
	int dec_bss=0;
	int dec_data=0;
	/*int dec_symb=0;*/
	File G = F->suiv;
    	enum {INIT, DONNE, DONNE_DATA, DONNE_BSS, DEBUT,  ETIQUETTE, INSTRUCTION_TEXT};
    /*etat de l'automate*/
    	int S = INIT;
	do{
		Donnee1* data=malloc(sizeof(data));
		Donnee2* bss=malloc(sizeof(bss));
		Instruction* nouvInstr=malloc(sizeof(nouvInstr));
		Symbole* nouvSymb=malloc(sizeof(nouvSymb));
    /*definition de machine aux etats*/
    	switch(S){
        	case INIT:
            		if(G->categorie == DIRECTIVE) 
				S = DONNE;
            		else if(G->categorie == SYMBOLE) 
				S = DEBUT;
			else if(G->categorie== COMMENTAIRE)
				G=G->suiv;
			else{
				printf("erreur ligne %d \n", G->ligne);
				G=G->suiv;
			}
            		break;

		case DONNE:
            		if(strcmp(G->lexeme, ".data")==0){
				S = INIT;
				Sect=DATA;
				G=G->suiv;
			}
            		else if(strcmp(G->lexeme,".bss")==0){
				S = INIT;
				Sect=DATA;
				G=G->suiv;
			}
			else if(strcmp(G->lexeme,".text")==0){
				S = INIT;
				Sect=TEXT;
				G=G->suiv;
			}
            		else if(strcmp(G->lexeme,".set")==0 && strcmp(G->suiv->lexeme,"noreorder")==0){
				S = INIT;
				G=G->suiv->suiv;
			}
			else if(Sect==DATA && ( strcmp(G->lexeme, ".byte")==0 || strcmp(G->lexeme, ".asciiz")==0 || strcmp(G->lexeme, ".word")==0 || strcmp(G->lexeme, ".space")==0))
				S = DONNE_DATA;
			else if(Sect==BSS && strcmp(G->lexeme, ".space")==0)
				S = DONNE_BSS;
			else{
				printf("erreur symbole de directive arreter l'assemblage ligne %d \n", G->ligne);
				G=G->suiv;
			}
            		break;

        	case DONNE_DATA:
			Sect=DATA;
			ListeG ope=NULL;
			*data=creerDonnee1(G->lexeme, G->categorie, 0, G->ligne, dec_data, ope);
			Do1=ajouterQueue(data, Do1);
			if(strcmp(G->lexeme, ".byte")==0){
				G=G->suiv;
				while (G->ligne==data->ligne){/*prendre en compte la position des virgules entre les operandes a corriger*/
					if(G->categorie==VIRGULE)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if((G->categorie==OCTATE) || (G->categorie==DECIMAL)){
							if((atoi(G->lexeme)>-129) && (atoi(G->lexeme)<128)){
								data->nbop+=1;
								Opedonnee* oper=malloc(sizeof(*oper));
								oper->word=atoi(G->lexeme);
								data->op=ajouterQueue(oper, data->op);
								dec_data+=1;
							}
							else
								printf("erreur ne tient pas sur un octet ligne %d \n",G->ligne);
					}
					else if((G->categorie==HEXA)){
						if(strlen(G->lexeme)>4)
							printf("erreur ne tient pas sur un octet ligne %d \n",G->ligne);
						else{
							data->nbop+=1;
							Opedonnee* oper=malloc(sizeof(*oper));
							strcpy(oper->as_et,G->lexeme);
							data->op=ajouterQueue(oper, data->op);
							dec_data+=1;
						}
					}	
					else
						printf("erreur l'opérande n'est pas du bon type pour la directive byte ligne %d \n",G->ligne);
					G=G->suiv;
				}
			}
			else if(strcmp(G->lexeme, ".asciiz")==0){
				G=G->suiv;
				while (G->ligne==data->ligne){
					if(G->categorie==VIRGULE)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if(G->categorie==CITATION){
						data->nbop+=1;
						Opedonnee* oper=malloc(sizeof(*oper));
						oper->as_et=G->lexeme;
						data->op=ajouterQueue(oper, data->op);
						dec_data+=strlen(G->lexeme)+1;
						G=G->suiv;
					}
					else{
						printf("erreur l'opérande n'est pas une citation ligne %d \n",G->ligne);
						G=G->suiv;
					}
				}
			}
			else if(strcmp(G->lexeme, ".word")==0){
				if(dec_data%4!=0){
					dec_data=4*((dec_data/4)+1);
					data->decalage=dec_data;
				}
				G=G->suiv;
				while (G->ligne==data->ligne){
					if(G->categorie==VIRGULE)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if((G->categorie==OCTATE) || (G->categorie==DECIMAL)){
						data->nbop+=1;
						Opedonnee* oper=malloc(sizeof(*oper));
						oper->word=atoi(G->lexeme);
						data->op=ajouterQueue(oper, data->op);
						dec_data+=4;
						G=G->suiv;
					}
					else if((G->categorie==HEXA)){
						data->nbop+=1;
						Opedonnee* oper=malloc(sizeof(*oper));
						strcpy(oper->as_et,G->lexeme);
						data->op=ajouterQueue(oper, data->op);
						dec_data+=4;
						G=G->suiv;
					}	
					else{
							printf("erreur l'opérande n'est pas du bon type pour la directive .word ligne %d \n",G->ligne);
							G=G->suiv;
					}
				}
			}
			else if(strcmp(G->lexeme, ".space")==0){
				G=G->suiv;
				while (G->ligne==data->ligne){
					if(G->categorie==VIRGULE)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else{
						if((G->categorie!=OCTATE) && (G->categorie!=DECIMAL))
							printf("erreur l'opérande n'est pas un chiffre pour la directive space ligne %d \n",G->ligne);
						else{
							data->nbop+=1;/*a verifier si plusieurs operandes possible*/
							Opedonnee* oper=malloc(sizeof(*oper));
							oper->word=atoi(G->lexeme);
							data->op=ajouterQueue(oper, data->op);
							dec_data+=atoi(G->lexeme);
						}
						G=G->suiv;
					}
				}
			}
			printf("datjyga");
			free(nouvInstr);
			printf("1");
			free(bss);
			printf("2");
			free(nouvSymb);
			printf("3 \n");
			S=INIT;
            		break;

        	case DONNE_BSS:
			Sect=BSS;
			*bss=creerDonnee2(G->lexeme, G->categorie, 0, G->ligne, dec_bss, 0);
			Do2=ajouterQueue(bss, Do2);
			G=G->suiv;
			while (G->ligne==bss->ligne){
					if(G->categorie==VIRGULE)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else{
						if((G->categorie!=OCTATE) && (G->categorie!=DECIMAL))
							printf("erreur l'opérande n'est pas un chiffre pour la directive space ligne %d \n",G->ligne);
						else{
							bss->nbop+=1;
							bss->valeur+=atoi(G->lexeme);
							dec_bss+=atoi(G->lexeme);
						}
						G=G->suiv;
					}
			}			
			free(data);
			free(nouvInstr);
			free(nouvSymb);
			S=INIT;
            		break;

        	case DEBUT: 
            		if(G->suiv->categorie==DEUX_POINTS){ 
				S = ETIQUETTE;
			}
            		else S = INSTRUCTION_TEXT;
            		break;

        	case ETIQUETTE:/*prendre en compte le cas ou l'etiquette existe deja*/
			/*;
			ListeG test=Symb->suiv;
			int k=0;
			do{
				if( strcmp(G->lexeme,test->pval->lexeme)==0){*//*a modifier*//*
					printf("erreur cette étiquette existe deja");
					k+=1;
				}
				test=test->suiv;
			}while(test!=Symb->suiv);
			if(k==0){*/
				*nouvSymb=creerSymbole(G->lexeme, G->categorie, G->ligne, Sect ,0);
				Symb=ajouterQueue(nouvSymb, Symb);
				switch(Sect){
					case(TEXT):
						nouvSymb->decalage=dec_text;
						break;
					case(BSS):
						nouvSymb->decalage=dec_bss;
						break;
					case(DATA):
						nouvSymb->decalage=dec_data;
						File test1=G->suiv;
						while((test1->categorie==COMMENTAIRE) && (test1!=F->suiv))
							test1=test1->suiv;
						if(strcmp(test1->lexeme,".word")==0)
							if(dec_data%4!=0){
								dec_data=((dec_data/4)+1)*4;
								nouvSymb->decalage=dec_data;
							}
						break;
				}
			/*}
			else
				free(nouvSymb);*/
			G=G->suiv->suiv;
			free(data);
			free(bss);
			free(nouvInstr);
			S=INIT;
            		break;

        	case INSTRUCTION_TEXT:
			Sect=TEXT;
			toLowerStr(G->lexeme);
			if(strcmp(G->lexeme,tableau[funHash(G->lexeme, taille)].symbole)!=0){
				printf("erreur, l'instruction n'existe pas ligne %d \n", G->ligne);
				File H=G;
				while (G->ligne==H->ligne)
					G=G->suiv;
				S=INIT;
				free(nouvInstr);
			}
			else{
				*nouvInstr=creerInstruction(G->lexeme, G->categorie,tableau[funHash(G->lexeme, taille)].operands, G->ligne, dec_text);
				Inst=ajouterQueue(nouvInstr, Inst);
				int i=0;
				G=G->suiv;
				while (G->ligne==nouvInstr->ligne){
					if(G->categorie==VIRGULE){
						G=G->suiv;
					}
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if(i>=tableau[funHash(G->lexeme, taille)].operands){
						printf("erreur il y a trop d'opérande à l'instruction ligne %d \n", G->ligne);/*on garde que les premiers operandes*/
						G=G->suiv;
					}
					else{
						nouvInstr->op[i].categorie=G->categorie;
						nouvInstr->op[i].lexeme=strdup(G->lexeme);
						i+=1;
						G=G->suiv;
					}
				}
				if(i<nouvInstr->nbop)
					printf("erreur il n'y a pas assez d'opérande à l'instruction ligne %d \n", nouvInstr->ligne);
				else
					dec_text+=4;
				free(data);
				free(bss);
				free(nouvSymb);
				S=INIT;
			}
            	break;  
    	}     
	}while(G!=F->suiv);
}


void gramAnalyse(File F, ListeG Inst, ListeG Symb, ListeG Do1, ListeG Do2){ 
	
	FILE* dictionnaire;

	dictionnaire = fopen("src/dictionnaire_instruction.txt","r");
	if (dictionnaire == NULL){
		printf("le dictionnaire n'a pas été ouvert \n"); 
		return; /*gestion erreurs*/
	}
	int nombreInstruc;
	fscanf(dictionnaire, "%d",&nombreInstruc);    
	if(nombreInstruc == EOF) return;/*gestion erreurs*/
	Dico hashTable[nombreInstruc];
	int index;
	char* instruc=malloc(sizeof(*instruc));
	char ty;
	int ope;
	int i = 0;
	for(i; i<nombreInstruc; i++){		
        	fscanf(dictionnaire, "%s %c %d", instruc, &ty, &ope);
		index = funHash(instruc,nombreInstruc);
		hashTable[index].symbole=strdup(instruc);
		hashTable[index].type=ty;
		hashTable[index].operands=ope;
	}
	fclose(dictionnaire);
	
	machine_a_etat_gram (F, Inst, Symb, Do1, Do2, hashTable, nombreInstruc);

}
