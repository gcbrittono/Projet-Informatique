#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>
#include <gram.h>
#include <lex.h>
#include <rel.h>
#include <unistd.h>
#include <math.h>
#include <global.h>
#include <notify.h>


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
	ListeG A=(ListeG)calloc(1,sizeof(struct el));
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
	for(i=0;i<L->nbop;i++)
		printf(" %s / ",L->op[i].lexeme);
	printf("\n---------------------------------------------------------------------------\n");
}

/*affiche la liste de donnée data*/
void afficherDo1(Donnee1* L){
	printf("Décalage %d : [ SYMBOLE ] : %s : nombre operande : %d : opérandes : ",L->decalage, L->lexeme, L->nbop);
	ListeG o=L->op->suiv;
	Opedonnee d;
	int i=0;
	for(i=0;i<L->nbop;i++){
		d=((OpeD*)(o->pval))->valeur;
		if(((OpeD*)(o->pval))->type==SYMBOLE || ((OpeD*)(o->pval))->type==CITATION)
			printf(" %s / ",d);
		else
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
		case 3:
			strcpy(s,"UNDEFINED");
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
	for(i=0; i < len; ++i)
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



void machine_a_etat_gram (File F, ListeG* Inst, ListeG* Symb, ListeG* Do1, ListeG* Do2, Dico tableau[], int taille, int* erreur){
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
            		if(G->categorie == DIRECTIVE){
				S = DONNE;
				goto donnee;
			}			
          		else if(G->categorie == SYMBOLE){
				S = DEBUT;
				goto debut;
			}
			else if(G->categorie== COMMENTAIRE)
				G=G->suiv;
			else{
				WARNING_MSG("erreur la ligne ne peut pas débuter par ce caractère ligne %d", G->ligne);
				G=G->suiv;
				*erreur =1;
			}
            		break;

		donnee :case DONNE:
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
				G=G->suiv;
				G=G->suiv;
			}
			else if(Sect==DATA && ( strcmp(G->lexeme, ".byte")==0 || strcmp(G->lexeme, ".asciiz")==0 || strcmp(G->lexeme, ".word")==0 || strcmp(G->lexeme, ".space")==0))
				S = DONNE_DATA;
			else if(Sect==BSS && strcmp(G->lexeme, ".space")==0)
				S = DONNE_BSS;
			else{
				WARNING_MSG("erreur symbole de directive arreter l'assemblage ligne %d", G->ligne);
				File K=G;
				*erreur =1;
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
				while (G->ligne==((Donnee1*)((*Do1)->pval))->ligne){
					if(G->categorie==VIRGULE && G->suiv->ligne==((Donnee1*)((*Do1)->pval))->ligne && G->suiv->categorie!=VIRGULE && ((Donnee1*)((*Do1)->pval))->nbop!=0)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if ((G->suiv->suiv->ligne==((Donnee1*)((*Do1)->pval))->ligne && G->suiv->categorie==VIRGULE) || G->suiv->ligne!=((Donnee1*)((*Do1)->pval))->ligne){
					if((G->categorie==OCTATE) || (G->categorie==DECIMAL) || (G->categorie==HEXA)){/*modifier pour les octate ne marche pas*/
							if((strtol(G->lexeme,NULL,0)>-129) && (strtol(G->lexeme,NULL,0)<128)){
								((Donnee1*)((*Do1)->pval))->nbop+=1;
								OpeD* oper=malloc(sizeof(*oper));
								oper->valeur.word=strtol(G->lexeme,NULL,0);
								oper->type=G->categorie;
								((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
								dec_data+=1;
							}
							else{
								WARNING_MSG("erreur ne tient pas sur un octet ligne %d",G->ligne);
								*erreur =1;
							}
					}
					/*else if((G->categorie==HEXA)){
						if(strlen(G->lexeme)>4){
							WARNING_MSG("erreur ne tient pas sur un octet ligne %d",G->ligne);
							*erreur =1;
						}
						else{
							((Donnee1*)((*Do1)->pval))->nbop+=1;
							OpeD* oper=malloc(sizeof(*oper));
							oper->valeur.as_et=strdup(G->lexeme);
							oper->type=G->categorie;
							((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
							dec_data+=1;
						}
					}*/
					else{
						WARNING_MSG("erreur l'opérande n'est pas du bon type pour la directive byte ligne %d",G->ligne);
						*erreur =1;
					}
					}
					else{
						WARNING_MSG("erreur d'alternance opérandes et virgules ligne %d",G->ligne);
						*erreur =1;
					}
					G=G->suiv;
				}
			}
			else if(strcmp(G->lexeme, ".asciiz")==0){
				G=G->suiv;
				while (G->ligne==((Donnee1*)((*Do1)->pval))->ligne){
					if(G->categorie==VIRGULE && G->suiv->ligne==((Donnee1*)((*Do1)->pval))->ligne && G->suiv->categorie!=VIRGULE && ((Donnee1*)((*Do1)->pval))->nbop!=0)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if ((G->suiv->suiv->ligne==((Donnee1*)((*Do1)->pval))->ligne && G->suiv->categorie==VIRGULE) || G->suiv->ligne!=((Donnee1*)((*Do1)->pval))->ligne){
					if(G->categorie==CITATION){
						((Donnee1*)((*Do1)->pval))->nbop+=1;
						OpeD* oper=malloc(sizeof(*oper));
						oper->valeur.as_et=strdup(G->lexeme);
						oper->type=G->categorie;
						((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
						dec_data+=strlen(G->lexeme)+1;
						G=G->suiv;
					}
					else{
						WARNING_MSG("erreur l'opérande n'est pas une citation ligne %d",G->ligne);
						G=G->suiv;
						*erreur =1;
					}
					}
					else{
						WARNING_MSG("erreur d'alternance opérandes et virgules ligne %d",G->ligne);
						*erreur =1;
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
					if(G->categorie==VIRGULE && G->suiv->ligne==((Donnee1*)((*Do1)->pval))->ligne && G->suiv->categorie!=VIRGULE && ((Donnee1*)((*Do1)->pval))->nbop!=0)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if ((G->suiv->suiv->ligne==((Donnee1*)((*Do1)->pval))->ligne && G->suiv->categorie==VIRGULE) || G->suiv->ligne!=((Donnee1*)((*Do1)->pval))->ligne){
					if((G->categorie==OCTATE) || (G->categorie==DECIMAL)){/*ne marche pas pour les octate*/
						((Donnee1*)((*Do1)->pval))->nbop+=1;
						OpeD* oper=malloc(sizeof(*oper));
						oper->valeur.word=atoi(G->lexeme);
						oper->type=G->categorie;
						((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
						dec_data+=4;
						G=G->suiv;
					}
					else if((G->categorie==HEXA)){
						((Donnee1*)((*Do1)->pval))->nbop+=1;
						OpeD* oper=malloc(sizeof(*oper));
						oper->valeur.as_et=strdup(G->lexeme);
						oper->type=G->categorie;
						((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
						dec_data+=4;
						G=G->suiv;
					}
					else if((G->categorie==SYMBOLE)){
						((Donnee1*)((*Do1)->pval))->nbop+=1;
						OpeD* oper=malloc(sizeof(*oper));
						oper->valeur.as_et=strdup(G->lexeme);
						oper->type=G->categorie;
						((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
						dec_data+=4;
						G=G->suiv;
					}
					else{
							WARNING_MSG("erreur l'opérande n'est pas du bon type pour la directive .word ligne %d",G->ligne);
							G=G->suiv;
							*erreur =1;
					}
					}
					else{
						WARNING_MSG("erreur d'alternance opérandes et virgules ligne %d",G->ligne);
						*erreur =1;
					}
				}
			}
			else if(strcmp(G->lexeme, ".space")==0){
				G=G->suiv;
				while (G->ligne==((Donnee1*)((*Do1)->pval))->ligne){
					if(G->categorie==VIRGULE && G->suiv->ligne==((Donnee1*)((*Do1)->pval))->ligne && G->suiv->categorie!=VIRGULE && ((Donnee1*)((*Do1)->pval))->nbop!=0)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if ((G->suiv->suiv->ligne==((Donnee1*)((*Do1)->pval))->ligne && G->suiv->categorie==VIRGULE) || G->suiv->ligne!=((Donnee1*)((*Do1)->pval))->ligne){
						if((G->categorie!=OCTATE) && (G->categorie!=DECIMAL) && (G->categorie!=HEXA)){
							WARNING_MSG("erreur l'opérande n'est pas un chiffre pour la directive space ligne %d",G->ligne);
							*erreur =1;
						}
						else{
							((Donnee1*)((*Do1)->pval))->nbop+=1;/*a verifier si plusieurs operandes possible*/
							OpeD* oper=malloc(sizeof(*oper));
							oper->valeur.word=strtol(G->lexeme,NULL,0);
							oper->type=G->categorie;
							((Donnee1*)((*Do1)->pval))->op=ajouterQueue(oper, ((Donnee1*)((*Do1)->pval))->op);
							dec_data+=strtol(G->lexeme,NULL,0);
						}
						G=G->suiv;
					}
					else{
							WARNING_MSG("erreur l'opérande n'est pas du bon type pour la directive .word ligne %d",G->ligne);
							G=G->suiv;
							*erreur =1;
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
					if(G->categorie==VIRGULE && G->suiv->ligne==((Donnee2*)((*Do2)->pval))->ligne && G->suiv->categorie!=VIRGULE && ((Donnee2*)((*Do2)->pval))->nbop!=0)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if ((G->suiv->suiv->ligne==((Donnee2*)((*Do2)->pval))->ligne && G->suiv->categorie==VIRGULE) || G->suiv->ligne!=((Donnee2*)((*Do2)->pval))->ligne){
						if((G->categorie!=OCTATE) && (G->categorie!=DECIMAL) && (G->categorie!=HEXA)){
							*erreur =1;
							WARNING_MSG("erreur l'opérande n'est pas un chiffre pour la directive space ligne %d",G->ligne);
						}
						else{
							((Donnee2*)((*Do2)->pval))->nbop+=1;
							((Donnee2*)((*Do2)->pval))->valeur+=strtol(G->lexeme,NULL,0);
							dec_bss+=strtol(G->lexeme,NULL,0);
						}
						G=G->suiv;
					}
					else{
							WARNING_MSG("erreur l'opérande n'est pas du bon type pour la directive .word ligne %d",G->ligne);
							G=G->suiv;
							*erreur =1;
					}
			}
			S=INIT;
            		break;

        	debut : case DEBUT:
            		if(G->suiv->categorie==DEUX_POINTS)
				S = ETIQUETTE;
            		else if(Sect==TEXT)
				S = INSTRUCTION_TEXT;
			else{
				WARNING_MSG("erreur instruction n'est pas dans la section TEXT arreter l'assemblage ligne %d", G->ligne);
				File O=G;
				*erreur =1;
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
						WARNING_MSG("erreur l'étiquette existe deja ligne %d",G->ligne);
						k+=1;
						*erreur =1;
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
			File H=G;
			/*printf("postion %d \n",funHash(G->lexeme, taille));*/
			if((tableau[funHash(G->lexeme, taille)].col==-1)){
				WARNING_MSG("erreur1, l'instruction ligne %d  n'existe pas", G->ligne);
				*erreur =1;
				while (G->ligne==H->ligne)
					G=G->suiv;
				S=INIT;
			}
			else if((strcmp(G->lexeme,tableau[funHash(G->lexeme, taille)].symbole)!=0) && (tableau[funHash(G->lexeme, taille)].col==-2)){
				WARNING_MSG("erreur2, l'instruction ligne %d  n'existe pas", G->ligne);
				*erreur =1;
				while (G->ligne==H->ligne)
					G=G->suiv;
				S=INIT;
			}
			else if((strcmp(G->lexeme,tableau[funHash(G->lexeme, taille)].symbole)!=0) && (tableau[funHash(G->lexeme, taille)].col>0) && (strcmp(G->lexeme,tableau[tableau[funHash(G->lexeme, taille)].col].symbole)!=0)){
				WARNING_MSG("erreur3, l'instruction ligne %d  n'existe pas", G->ligne);
				*erreur =1;
				printf("%s -- ", tableau[tableau[funHash(G->lexeme, taille)].col].symbole);
				printf("%s -- ",tableau[funHash(G->lexeme, taille)].symbole);
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
					if(G->categorie==VIRGULE && G->suiv->ligne==((Instruction*)((*Inst)->pval))->ligne && G->suiv->categorie!=VIRGULE && i!=0)
						G=G->suiv;
					else if(G->categorie==COMMENTAIRE)
						G=G->suiv;
					else if(i>=tableau[position].operands){
						WARNING_MSG("erreur il y a trop d'opérande à l'instruction ligne %d", G->ligne);/*on garde que les premiers operandes*/
						G=G->suiv;
						*erreur =1;
					}
					else if((((i<(((Instruction*)((*Inst)->pval))->nbop)-1) && (G->suiv->categorie==VIRGULE))) || (i==(((Instruction*)((*Inst)->pval))->nbop)-1 && (G->categorie!=VIRGULE))){
						((Instruction*)((*Inst)->pval))->op[i].categorie=G->categorie;
						((Instruction*)((*Inst)->pval))->op[i].lexeme=strdup(G->lexeme);
						((Instruction*)((*Inst)->pval))->op[i].typeadr=strdup(tableau[position].type_op[i]);
						i+=1;
						G=G->suiv;
					}
					else {
						WARNING_MSG("probleme d'alternance avec les virgules ligne %d", G->ligne);
						*erreur =1;
						printf("%d--", i);
						printf("%d--", G->categorie);
						while (G->ligne==H->ligne)
							G=G->suiv;
					}
					/*probleme alternance operande virgule*/
				}
				if(i<((Instruction*)((*Inst)->pval))->nbop){
					WARNING_MSG("erreur il n'y a pas assez d'opérande à l'instruction ligne %d", ((Instruction*)((*Inst)->pval))->ligne);
					*erreur =1;
				}
				else
					dec_text+=4;
			}

			S=INIT;
            	break;
    	}
	}while(G!=F->suiv);
/*printf("%s\n",G->lexeme);
printf("%s\n",F->lexeme);
printf("%p\n",G);
printf("%p\n",F);*/
}


void gramAnalyse(File F, ListeG* Inst, ListeG* Symb, ListeG* Do1, ListeG* Do2, int* erreur){

	FILE* dictionnaire;

	dictionnaire = fopen("src/dictionnaire_instruction.txt","r");
	if (dictionnaire == NULL){
		ERROR_MSG("le dictionnaire n'a pas été ouvert");
	}
	int nombreInstruc;
	fscanf(dictionnaire, "%d",&nombreInstruc);
	if(nombreInstruc == EOF) ERROR_MSG("le dictionnaire est vide");
	Dico hashTable[60];
	int index;
	/*char* instruc=malloc(sizeof(*instruc));*/
	char instruc[10];
	char ty;
	int ope;
	char o1[3];
	int i = 0;
	int j=0;
	int k=0;
	for (i=0;i<60;i++)
		hashTable[i].col=-1;
	for(i=0; i<nombreInstruc; i++){
        	fscanf(dictionnaire, "%s %c %d", instruc, &ty, &ope);
		index = funHash(instruc,nombreInstruc);
		if(hashTable[index].col==-1){
			hashTable[index].symbole=strdup(instruc);
			hashTable[index].type=ty;
			hashTable[index].operands=ope;
			hashTable[index].col=-2;
		}
		else if(hashTable[index].col==-2) {
			hashTable[index].col=50+j;
			hashTable[50+j].col=-2;
			index=50+j;
			j+=1;
			hashTable[index].symbole=strdup(instruc);
			hashTable[index].type=ty;
			hashTable[index].operands=ope;
		}
/*chargement du type d'adressage des registres*/
		for(k=0;k<ope;k++){
			fscanf(dictionnaire, "%s ",o1);
			hashTable[index].type_op[k]=strdup(o1);
		}
	}
	fclose(dictionnaire);

	machine_a_etat_gram (F, Inst, Symb, Do1, Do2, hashTable, nombreInstruc, erreur);
	libererdico(hashTable, nombreInstruc);

}
