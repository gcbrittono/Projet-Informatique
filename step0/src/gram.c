#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>
#include <gram.h>
#include <lex.h>
#include <unistd.h>
#include <math.h>


/*fonction qui crée une liste*/
ListeG creerListeG(){
	return NULL;
}

/*fonction qui vérifie si la liste est vide*/
int listeVide(ListeG L){
	return !L;
}


Instruction* creerInstruction(char* lex, etat cat,int nombop, int lig,unsigned int dec , char type/*,ListeG operande*/){
	Instruction* p=malloc(sizeof(*p));
	p->nom=strdup(lex);
	p->type=cat;
	p->nbop=nombop;
	p->ligne=lig;
	p->decalage=dec;
	p->type_inst=type;
	/*p.op=operande;*/
	return p;
}

Donnee1* creerDonnee1(char* lex, etat cat, int nombop, int lig, int dec, ListeG operande){
	Donnee1* p=malloc(sizeof(*p));
	p->lexeme=strdup(lex);
	p->type=cat;
	p->nbop=nombop;
	p->ligne=lig;
	p->decalage=dec;
	p->op=operande;
	return p;
}

Donnee2* creerDonnee2(char* lex, etat cat, int nombop, int lig, int dec, int val){
	Donnee2* p=malloc(sizeof(*p));
	p->lexeme=strdup(lex);
	p->type=cat;
	p->nbop=nombop;
	p->ligne=lig;
	p->decalage=dec;
	p->valeur=val;
	return p;
}

Symbole* creerSymbole(char* lex,	etat cat, int lig, Section section ,int dec){
	Symbole* p=malloc(sizeof(*p));
	p->lexeme=strdup(lex);
	p->type=cat;
	p->ligne=lig;
	p->decalage=dec;
	p->sect=section;
	return p;
}

ListeG ajouterQueue(void* e, ListeG L){
	ListeG A=calloc(1,sizeof(A)/*sizeof(*e)+sizeof(A->suiv)*/);
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

/*affiche la liste d'instruction*/
void afficherInst(Instruction* L){
	printf("Décalage %d : [ SYMBOLE ] : %s : nombre operande : %d : opérandes : ",L->decalage, L->nom, L->nbop);
	int i=0;
	for(i;i<=L->nbop-1;i++)
		printf(" %s / ",L->op[i].lexeme);
	printf("\n---------------------------------------------------------------------------\n");
}

/*affiche la liste de donnée data*/
void afficherDo1(Donnee1* L){
	printf("Décalage %d : [ SYMBOLE ] : %s : nombre operande : %d : opérandes : ",L->decalage, L->lexeme, L->nbop);
	ListeG o=L->op;
	Opedonnee* d;
	int i=0;
	for(i;i<=L->nbop-1;i++){
		d=(Opedonnee*)(o->pval);
		printf(" %u / ",d);
		o=o->suiv;
	}
	/*for(o;o==L->op;o=o->suiv){
		d=(Opedonnee*)(o->pval);
		printf("%u ",*d);
	}
	d=(Opedonnee*)(o->pval);
	printf("%u ",*d);*/
	printf("\n---------------------------------------------------------------------------\n");
}
/*affichage de la liste de données bss*/
void afficherDo2(Donnee2* L){
	printf("Décalage %d : [ SYMBOLE ] : %s : nombre operande : %d : opérandes : %d",L->decalage, L->lexeme, L->nbop, L->valeur);
	printf("\n---------------------------------------------------------------------------\n");
}

/*affichage de la liste de symboles*/
void afficherSymb(Symbole* L){
	char s[5];
	switch(L->sect){
		case 0:
			strcpy(s,"DATA");
			break;
		case 1:
			strcpy(s,"BSS");
			break;
		case 2:
			strcpy(s,"TEXT");
			break;
	}
	printf("Section %s : [ ETIQUETTE ] : %s : décalage : %d",s, L->lexeme, L->decalage);
	printf("\n---------------------------------------------------------------------------\n");
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


int funHash(char* str, int taille){
	char* new=strdup(str);
	toLowerStr(new);
	long hash=new[0];
	int i=1;
	int len = strlen(new);
	for(i; i < len; ++i)
		hash +=(((int)(89*pow(67,i)))%(50-i))*new[i];
	hash=hash%50;
	return hash;
}
/*Fonction utilisée dans la fonction funHash pour convertir les character string en son equivalent minuscule*/
void toLowerStr(char *str){
	int lenght = strlen(str);
	int i;
	for(i = 0; i < lenght; i++){
		str[i] = tolower(str[i]);
	}
}



void machine_a_etat_gram (File F, ListeG* Inst, ListeG* Symb, ListeG* Do1, ListeG* Do2, Dico tableau[], int taille){
    /*definition des etats et des décalages dans chaque section*/
	Section Sect;
	int dec_text=0;
	int dec_bss=0;
	int dec_data=0;
	/*int dec_symb=0;*/
	/*pour tester si l'étiquette existe deja*/
	int k;
	/*fin test*/
	File G = F->suiv;
    	enum {INIT, DONNE, DONNE_DATA, DONNE_BSS, DEBUT,  ETIQUETTE, INSTRUCTION_TEXT};
    /*etat de l'automate*/
    	int S = INIT;
	do{
	/*printf("ligne de travail %d \n", G->ligne);*/
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
				Sect=BSS;
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
				File K=G;
				while (G->ligne==K->ligne)
					G=G->suiv;
			}
            		break;

        	case DONNE_DATA:
			Sect=DATA;
			ListeG ope=NULL;
			*Do1=ajouterQueue(creerDonnee1(G->lexeme, G->categorie, 0, G->ligne, dec_data, ope), *Do1);
			if(strcmp(G->lexeme, ".byte")==0){
				G=G->suiv;
				while (G->ligne==((Donnee1*)((*Do1)->pval))->ligne){/*prendre en compte la position des virgules entre les operandes a corriger*/
					if(G->categorie==VIRGULE)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if((G->categorie==OCTATE) || (G->categorie==DECIMAL)){
							if((atoi(G->lexeme)>-129) && (atoi(G->lexeme)<128)){
								((Donnee1*)((*Do1)->pval))->nbop+=1;
								Opedonnee* oper=malloc(sizeof(*oper));
								oper->word=atoi(G->lexeme);
								((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
								dec_data+=1;
							}
							else
								printf("erreur ne tient pas sur un octet ligne %d \n",G->ligne);
					}
					else if((G->categorie==HEXA)){
						if(strlen(G->lexeme)>4)
							printf("erreur ne tient pas sur un octet ligne %d \n",G->ligne);
						else{
							((Donnee1*)((*Do1)->pval))->nbop+=1;
							Opedonnee* oper=malloc(sizeof(*oper));
							strcpy(oper->as_et,G->lexeme);
							((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
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
				while (G->ligne==((Donnee1*)((*Do1)->pval))->ligne){
					if(G->categorie==VIRGULE)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if(G->categorie==CITATION){
						((Donnee1*)((*Do1)->pval))->nbop+=1;
						Opedonnee* oper=malloc(sizeof(*oper));
						oper->as_et=G->lexeme;
						((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
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
					((Donnee1*)((*Do1)->pval))->decalage=dec_data;
				}
				G=G->suiv;
				while (G->ligne==((Donnee1*)((*Do1)->pval))->ligne){
					if(G->categorie==VIRGULE)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if((G->categorie==OCTATE) || (G->categorie==DECIMAL)){
						((Donnee1*)((*Do1)->pval))->nbop+=1;
						Opedonnee* oper=malloc(sizeof(*oper));
						oper->word=atoi(G->lexeme);
						((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
						dec_data+=4;
						G=G->suiv;
					}
					else if((G->categorie==HEXA)){
						((Donnee1*)((*Do1)->pval))->nbop+=1;
						Opedonnee* oper=malloc(sizeof(*oper));
						strcpy(oper->as_et,G->lexeme);
						((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
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
				while (G->ligne==((Donnee1*)((*Do1)->pval))->ligne){
					if(G->categorie==VIRGULE)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else{
						if((G->categorie!=OCTATE) && (G->categorie!=DECIMAL))
							printf("erreur l'opérande n'est pas un chiffre pour la directive space ligne %d \n",G->ligne);
						else{
							((Donnee1*)((*Do1)->pval))->nbop+=1;/*a verifier si plusieurs operandes possible*/
							Opedonnee* oper=malloc(sizeof(*oper));
							oper->word=atoi(G->lexeme);
							((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
							dec_data+=atoi(G->lexeme);
						}
						G=G->suiv;
					}
				}
			}
			S=INIT;
            		break;

        	case DONNE_BSS:
			Sect=BSS;
			*Do2=ajouterQueue(creerDonnee2(G->lexeme, G->categorie, 0, G->ligne, dec_bss, 0), *Do2);
			G=G->suiv;
			while (G->ligne==((Donnee2*)((*Do2)->pval))->ligne){
					if(G->categorie==VIRGULE)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else{
						if((G->categorie!=OCTATE) && (G->categorie!=DECIMAL))
							printf("erreur l'opérande n'est pas un chiffre pour la directive space ligne %d \n",G->ligne);
						else{
							((Donnee2*)((*Do2)->pval))->nbop+=1;
							((Donnee2*)((*Do2)->pval))->valeur+=atoi(G->lexeme);
							dec_bss+=atoi(G->lexeme);
						}
						G=G->suiv;
					}
			}			
			S=INIT;
            		break;

        	case DEBUT: 
            		if(G->suiv->categorie==DEUX_POINTS)
				S = ETIQUETTE;
            		else if(Sect==TEXT)
				S = INSTRUCTION_TEXT;
			else{
				printf("erreur instruction n'est pas dans la section TEXT arreter l'assemblage ligne %d \n", G->ligne);
				File O=G;
				while (G->ligne==O->ligne)
					G=G->suiv;
			}
            		break;

        	case ETIQUETTE:/*prendre en compte le cas ou l'etiquette existe deja*/
			k=0;
			ListeG test=*Symb;
			if (!listeVide(*Symb)){
				do{	
					test=test->suiv;
					if( strcmp(G->lexeme,((Symbole*)(test->pval))->lexeme)==0){
						printf("erreur l'étiquette existe deja ligne %d \n",G->ligne);
						k+=1;
						}
				}while(test!=(*Symb)->suiv);
			}
			if(k==0){
				*Symb=ajouterQueue(creerSymbole(G->lexeme, G->categorie, G->ligne, Sect ,0), *Symb);
				switch(Sect){
					case(TEXT):
						((Symbole*)((*Symb)->pval))->decalage=dec_text;
						break;
					case(BSS):
						((Symbole*)((*Symb)->pval))->decalage=dec_bss;
						break;
					case(DATA):
						((Symbole*)((*Symb)->pval))->decalage=dec_data;
						File test1=G->suiv;
						while((test1->categorie==COMMENTAIRE) && (test1!=F->suiv))
							test1=test1->suiv;
						if(strcmp(test1->lexeme,".word")==0)
							if(dec_data%4!=0){
								dec_data=((dec_data/4)+1)*4;
								((Symbole*)((*Symb)->pval))->decalage=dec_data;
							}
						break;
				}
			}	
			G=G->suiv->suiv;
			S=INIT;
            		break;

        	case INSTRUCTION_TEXT:
			Sect=TEXT;
			toLowerStr(G->lexeme);/*passe toutes les instructions en miniscule*/
			int position;
			/*printf("postion %d \n",funHash(G->lexeme, taille));*/
			if((tableau[funHash(G->lexeme, taille)].col==-1) || ((tableau[funHash(G->lexeme, taille)].col==-2) && (strcmp(G->lexeme,tableau[funHash(G->lexeme, taille)].symbole)!=0)) || ((tableau[funHash(G->lexeme, taille)].col>0) && (strcmp(G->lexeme,tableau[tableau[funHash(G->lexeme, taille)].col].symbole)!=0))){
				printf("erreur, l'instruction n'existe pas ligne %d \n", G->ligne);
				File H=G;
				while (G->ligne==H->ligne)
					G=G->suiv;
				S=INIT;
			}
			else{
				if(strcmp(G->lexeme,tableau[funHash(G->lexeme, taille)].symbole)==0)
					position=funHash(G->lexeme, taille);
				else
					position=tableau[funHash(G->lexeme, taille)].col;
				*Inst=ajouterQueue(creerInstruction(G->lexeme, G->categorie,tableau[position].operands, G->ligne, dec_text,tableau[position].type), *Inst);
				int i=0;
				if (strcmp(G->lexeme,"lw")==0 || strcmp(G->lexeme,"sw")==0 || strcmp(G->lexeme,"blt")==0)
					dec_text+=4;
				G=G->suiv;
				while (G->ligne==((Instruction*)((*Inst)->pval))->ligne){
					if(G->categorie==VIRGULE){
						G=G->suiv;
					}
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if(i>=tableau[position].operands){
						printf("erreur il y a trop d'opérande à l'instruction ligne %d \n", G->ligne);/*on garde que les premiers operandes*/
						G=G->suiv;
					}
					else{
						((Instruction*)((*Inst)->pval))->op[i].categorie=G->categorie;
						((Instruction*)((*Inst)->pval))->op[i].lexeme=strdup(G->lexeme);
						i+=1;
						G=G->suiv;
					}/*probleme alternance operande virgule*/
				}
				if(i<((Instruction*)((*Inst)->pval))->nbop)
					printf("erreur il n'y a pas assez d'opérande à l'instruction ligne %d \n", ((Instruction*)((*Inst)->pval))->ligne);
				else
					dec_text+=4;
			}

			S=INIT;
            	break;  
    	}     
	}while(G!=F->suiv);

}


void gramAnalyse(File F, ListeG* Inst, ListeG* Symb, ListeG* Do1, ListeG* Do2){ 
	
	FILE* dictionnaire;

	dictionnaire = fopen("src/dictionnaire_instruction.txt","r");
	if (dictionnaire == NULL){
		printf("le dictionnaire n'a pas été ouvert \n"); 
		return; /*gestion erreurs*/
	}
	int nombreInstruc;
	fscanf(dictionnaire, "%d",&nombreInstruc);    
	if(nombreInstruc == EOF) return;/*gestion erreurs*/
	Dico hashTable[60];
	int index;
	char* instruc=malloc(sizeof(*instruc));
	char ty;
	int ope;
	int i = 0;
	int j=0;
	type_operande type_hash [2];

	for (i=0;i<60;i++)
		hashTable[i].col=-1;
	for(i=0; i<nombreInstruc; i++){		
        	fscanf(dictionnaire, "%s %c %d %s %s %s", instruc, &ty, &ope, type_hash[0], type_hash[1], type_hash[2]);
		index = funHash(instruc,nombreInstruc);
		if(hashTable[index].col==-1){
			hashTable[index].symbole=strdup(instruc);
			hashTable[index].type=ty;
			hashTable[index].operands=ope;
			hashTable[index].col=-2;
			hashTable[index].typeO[0]=type_hash[0];
			hashTable[index].typeO[1]=type_hash[1];
			hashTable[index].typeO[2]=type_hash[2];
		}
		else if(hashTable[index].col==-2) {
			hashTable[index].col=50+j;
			hashTable[50+j].col=-2;
			index=50+j;
			j+=1;
			hashTable[index].symbole=strdup(instruc);
			hashTable[index].type=ty;
			hashTable[index].operands=ope;
			hashTable[index].typeO[0]=type_hash[0];
			hashTable[index].typeO[1]=type_hash[1];
			hashTable[index].typeO[2]=type_hash[2];
		}
	}
	fclose(dictionnaire);
	
	machine_a_etat_gram (F, Inst, Symb, Do1, Do2, hashTable, nombreInstruc);

}
