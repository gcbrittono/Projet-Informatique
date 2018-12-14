#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <gram.h>
#include <lex.h>
#include <rel.h>
#include <global.h>
#include <notify.h>


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
		ERROR_MSG("erreur ce registre n'existe pas ligne %d \n",ligne);/*gestion erreurs arreter compil?*/
}

ListeG inserer(void* e, ListeG L){
	ListeG A=calloc(1,sizeof(e)+sizeof(A->suiv));
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
/*------------------------fonction pour libérer la mémoire---------------------------------*/
void libererInstruction(ListeG* L){
	if(listeVide(*L))
		return;
	while((*L)->suiv!=*L){
		ListeG A=(*L)->suiv;
		free(((Instruction*)(A->pval))->nom);
		(*L)->suiv=(*L)->suiv->suiv;
		free(A);
	}
	free(((Instruction*)((*L)->pval))->nom);
	free(*L);
}

void libererDo1(ListeG* L){
	if(listeVide(*L))
		return;
	while((*L)->suiv!=*L){
		ListeG A=(*L)->suiv;
		free(((Donnee1*)(A->pval))->lexeme);
		(*L)->suiv=(*L)->suiv->suiv;
		free(A);
	}
	free(((Donnee1*)((*L)->pval))->lexeme);
	free(*L);
}

void libererDo2(ListeG* L){
	if(listeVide(*L))
		return;
	while((*L)->suiv!=*L){
		ListeG A=(*L)->suiv;
		free(((Donnee2*)(A->pval))->lexeme);
		(*L)->suiv=(*L)->suiv->suiv;
		free(A);
	}
	free(((Donnee2*)((*L)->pval))->lexeme);
	free(*L);
}

void libererSymbole(ListeG* L){
	if(listeVide(*L))
		return;
	while((*L)->suiv!=*L){
		ListeG A=(*L)->suiv;
		free(((Symbole*)(A->pval))->lexeme);
		(*L)->suiv=(*L)->suiv->suiv;
		free(A);
	}
	free(((Symbole*)((*L)->pval))->lexeme);
	free(*L);
}

void libererFile(File* L){
	if(estVide(*L))
		return;
	while((*L)->suiv!=*L){
		File A=(*L)->suiv;
		free(A->lexeme);
		(*L)->suiv=(*L)->suiv->suiv;
		free(A);
	}
	free((*L)->lexeme);
	free(*L);
}

void libererregistre(registres tab[], int taille){
	int i;
	for(i=0;i<taille;i++){
		free(tab[i].mnemo);
		free(tab[i].reg);
	}
}

void libererdico(Dico d[], int taille){
	int i;
	for(i=0;i<taille;i++){
		if(d[i].col!=-1){
			free(d[i].symbole);
			int j;
			for(j=0;j<d[i].operands;j++)
				free(d[i].type_op[j]);
		}
	}
}
/*------------------------------------------------------------------------------------*/

void associerReg(ListeG Inst,registres tableau[32],int ligne){
	int nb=((Instruction*)(Inst->pval))->nbop;
	int i = 0;
	for(i=0; i<nb;i++){
		if(((Instruction*)(Inst->pval))->op[i].categorie==REGISTRE)
			registre(((Instruction*)(Inst->pval))->op[i].lexeme,tableau,ligne);
		else if (((Instruction*)(Inst->pval))->op[i].categorie==BASE_OF){
			char* token = ((Instruction*)(Inst->pval))->op[i].lexeme;
			int j=0;
			while(*token!='('){
				token++;
				j+=1;
			}
			token++;
			int k=0;
			while(token[k]!=')')
				k+=1;
			char* reg;
			reg = strndup(token,k);
			registre(reg,tableau,ligne);
			char* nouveaux;
			nouveaux=strndup(((Instruction*)(Inst->pval))->op[i].lexeme,j);
			strcat(nouveaux,"(");
			strcat(nouveaux,reg);
			strcat(nouveaux,")");
			free(((Instruction*)(Inst->pval))->op[i].lexeme);
			((Instruction*)(Inst->pval))->op[i].lexeme=strdup(nouveaux);
		}

	}
}

void chargeRegistre(registres tab[32]){
FILE* re;

	re = fopen("src/registre.txt","r");
	if (re == NULL){
		ERROR_MSG("le dictionnaire de registre n'a pas été ouvert ");
		return; /*gestion erreurs*/
	}
	char reg1[10]/*=malloc(sizeof(*reg1))*/;
	char reg2[4]/*=malloc(sizeof(*reg2))*/;
	int i;
	for (i=0;i<32;i++){
        	fscanf(re, "%s %s",reg1,reg2);
		tab[i].mnemo=strdup(reg1);
		tab[i].reg=strdup(reg2);
	}
	fclose(re);
}


/*Fonction pour créer la table de relocation à partir de l'étiquette*/
table_relocation*  symbole_find(ListeG L, Symbole* symb, int i /*entier qui indique si data ou text*/){
	table_relocation* tab_rel= malloc(sizeof(*tab_rel));
	if(i==0){
	/*if(symb->sect==DATA){*/
     		/*tab_rel->nom_section =strdup("DATA");*/
		/*tab_rel->sect=DATA;*/
		tab_rel->sect=symb->sect;
            	tab_rel->addr_relative = ((Donnee1*)(L->pval))->decalage;/*symb->decalage*/
            	tab_rel->mode_relocation = 2;
		tab_rel->pointeur = symb;
	}
	else if (i==1){
        /*else if(symb->sect==TEXT){*/
            	/*tab_rel->nom_section =strdup("TEXT");*/
		tab_rel->sect=TEXT;
            	tab_rel->addr_relative = ((Instruction*)(L->pval))->decalage;/*symb->decalage*/
		switch(((Instruction*)(L->pval))->type_inst){
			case 'R'  :
      				tab_rel->mode_relocation = 2;
      				break;

   			case 'I'  :
      				tab_rel->mode_relocation = 5&6;
      				break;

   			case 'J'  :
      				tab_rel->mode_relocation = 4;
      				break;


   			default :
        			tab_rel->mode_relocation = 2;
		}
            	/*tab_rel.mode_relocation = 2; */ /*Je ne suis pas sur si ca marcherá dans le cas text*/
		tab_rel->pointeur = symb;
	}
	return tab_rel;
}


/*Fonction pour remplacer un autre instruction dans la liste*/
void remplacer_instr(ListeG listeInstr, char* nom_instr_final, int nombop_instr_final, char type_instr_final, char* ope[], etat typop[]){
	free(((Instruction*)(listeInstr->pval))->nom );
	((Instruction*)(listeInstr->pval))->nom = strdup(nom_instr_final);
	((Instruction*)(listeInstr->pval))->type_inst = type_instr_final;
	((Instruction*)(listeInstr->pval))->nbop = nombop_instr_final;
	int i=0;
	for(i=0;i<nombop_instr_final;i++){
		if(((Instruction*)(listeInstr->pval))->op[i].lexeme!=NULL)
			free(((Instruction*)(listeInstr->pval))->op[i].lexeme);
		((Instruction*)(listeInstr->pval))->op[i].lexeme = strdup(ope[i]);
		((Instruction*)(listeInstr->pval))->op[i].categorie=typop[i];
	}
}

/*Fonction qui remplace les pseudo instructions
la fonction décale le pointeur de la liste lorsqu'elle ajoute une instruction*/
void pseudoInstruction( ListeG* instr){
	char* o[3];
	etat typop[3];
	if (strcmp(((Instruction*)((*instr)->pval))->nom,"lw")==0){
		o[0]=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		o[1]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		typop[0]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[1]=((Instruction*)((*instr)->pval))->op[1].categorie;
		*instr=inserer(creerInstruction("lw", ((Instruction*)((*instr)->pval))->type, 2, ((Instruction*)((*instr)->pval))->ligne, (((Instruction*)((*instr)->pval))->decalage)+4 , 'I'), *instr);
		((Instruction*)((*instr)->suiv->pval))->op[0].categorie=((Instruction*)((*instr)->pval))->op[0].categorie;
		((Instruction*)((*instr)->suiv->pval))->op[0].lexeme=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		((Instruction*)((*instr)->suiv->pval))->op[1].categorie=SYMBOLE/*BASE_OF*/;
		char mot[256];
		strcpy(mot,((Instruction*)((*instr)->pval))->op[1].lexeme);/*ici*/
		strcat(mot,"(");
		strcat(mot,((Instruction*)((*instr)->pval))->op[0].lexeme);
		strcat(mot,")");
		((Instruction*)((*instr)->suiv->pval))->op[1].lexeme=strdup(mot);
		((Instruction*)((*instr)->suiv->pval))->op[0].typeadr=strdup("Reg");
		((Instruction*)((*instr)->suiv->pval))->op[1].typeadr=strdup("Bas");
		remplacer_instr(*instr, "lui", 2, 'I',o,typop);
		((Instruction*)((*instr)->pval))->op[0].typeadr=strdup("Reg");/*ici changer l'opérande*/
		((Instruction*)((*instr)->pval))->op[1].typeadr=strdup("Imm");
/*faire la relocation ici pour les deux instructions*/
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"sw")==0){
		o[0]=strdup("$1");
		o[1]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		typop[0]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[1]=((Instruction*)((*instr)->pval))->op[1].categorie;
		*instr=inserer(creerInstruction("sw", ((Instruction*)((*instr)->pval))->type, 2, ((Instruction*)((*instr)->pval))->ligne, (((Instruction*)((*instr)->pval))->decalage)+4 , 'I'), *instr);
		((Instruction*)((*instr)->suiv->pval))->op[0].categorie=((Instruction*)((*instr)->pval))->op[0].categorie;
		((Instruction*)((*instr)->suiv->pval))->op[0].lexeme=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		((Instruction*)((*instr)->suiv->pval))->op[1].categorie=SYMBOLE/*BASE_OF*/;
		char mot[256];
		strcpy(mot,((Instruction*)((*instr)->pval))->op[1].lexeme);
		strcat(mot,"($1)");
		((Instruction*)((*instr)->suiv->pval))->op[1].lexeme=strdup(mot);
		((Instruction*)((*instr)->suiv->pval))->op[0].typeadr=strdup("Reg");
		((Instruction*)((*instr)->suiv->pval))->op[1].typeadr=strdup("Bas");
		remplacer_instr(*instr, "lui", 2, 'I',o, typop);
		((Instruction*)((*instr)->pval))->op[0].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[1].typeadr=strdup("Imm");

/*faire la relocation ici pour les deux instructions*/
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"nop")==0){
		o[0]=strdup("$0");
		o[1]=strdup("$0");
		o[2]=strdup("0");
		typop[0]=REGISTRE;
		typop[1]=REGISTRE;
		typop[2]=DECIMAL;
		remplacer_instr(*instr, "sll", 3, 'I',o, typop);
		((Instruction*)((*instr)->pval))->op[0].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[1].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[2].typeadr=strdup("sa");
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"move")==0){
		o[0]=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		o[1]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		o[2]=strdup("$0");
		typop[0]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[1]=((Instruction*)((*instr)->pval))->op[1].categorie;
		typop[2]=REGISTRE;
		remplacer_instr(*instr, "add", 3, 'R',o, typop);
		((Instruction*)((*instr)->pval))->op[0].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[1].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[2].typeadr=strdup("Reg");
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"li")==0){
		o[0]=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		o[1]=strdup("$0");
		o[2]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		typop[0]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[1]=REGISTRE;
		typop[2]=((Instruction*)((*instr)->pval))->op[1].categorie;
		remplacer_instr(*instr, "addi", 3, 'I',o, typop);
		((Instruction*)((*instr)->pval))->op[0].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[1].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[2].typeadr=strdup("Imm");
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"blt")==0){
		o[0]=strdup("$1");
		o[1]=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		o[2]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		typop[0]=REGISTRE;
		typop[1]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[2]=((Instruction*)((*instr)->pval))->op[1].categorie;
		*instr=inserer(creerInstruction("bne", ((Instruction*)((*instr)->pval))->type, 3, ((Instruction*)((*instr)->pval))->ligne, (((Instruction*)((*instr)->pval))->decalage)+4 , 'I'), *instr);
		((Instruction*)((*instr)->suiv->pval))->op[0].categorie=REGISTRE;
		((Instruction*)((*instr)->suiv->pval))->op[0].lexeme=strdup("$1");
		((Instruction*)((*instr)->suiv->pval))->op[1].categorie=REGISTRE;
		((Instruction*)((*instr)->suiv->pval))->op[1].lexeme=strdup("$0");
		((Instruction*)((*instr)->suiv->pval))->op[2].categorie=((Instruction*)((*instr)->pval))->op[2].categorie;
		((Instruction*)((*instr)->suiv->pval))->op[2].lexeme=strdup(((Instruction*)((*instr)->pval))->op[2].lexeme);
		((Instruction*)((*instr)->suiv->pval))->op[0].typeadr=strdup("Reg");
		((Instruction*)((*instr)->suiv->pval))->op[1].typeadr=strdup("Reg");
		((Instruction*)((*instr)->suiv->pval))->op[2].typeadr=strdup("Rel");
		remplacer_instr(*instr, "slt", 3, 'R',o, typop);
		((Instruction*)((*instr)->pval))->op[0].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[1].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[2].typeadr=strdup("Reg");


/*faire la relocation ici*/
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"neg")==0){
		o[0]=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		o[1]=strdup("$0");
		o[2]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		typop[0]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[1]=REGISTRE;
		typop[2]=((Instruction*)((*instr)->pval))->op[1].categorie;
		remplacer_instr(*instr, "sub", 3, 'R',o, typop);
		((Instruction*)((*instr)->pval))->op[0].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[1].typeadr=strdup("Reg");
		((Instruction*)((*instr)->pval))->op[2].typeadr=strdup("Reg");
	}
}


void relocationInst(ListeG Inst,ListeG* Symb,int ligne,ListeG* RelocInst){
	int nb=((Instruction*)(Inst->pval))->nbop;
	int i = 0;
	for(i=0; i<nb;i++){
		if((((Instruction*)(Inst->pval))->op[i].categorie==SYMBOLE) && ((strcmp(((Instruction*)(Inst->pval))->op[i].typeadr,"Imm")==0 || strcmp(((Instruction*)(Inst->pval))->op[i].typeadr,"Abs")==0 || strcmp(((Instruction*)(Inst->pval))->op[i].typeadr,"Bas")==0))){
			if(strcmp(((Instruction*)(Inst->pval))->nom,"lw")!=0 && strcmp(((Instruction*)(Inst->pval))->nom,"sw")!=0)
				*RelocInst=ajouterQueue(symbole_find(Inst, trouverSymbole(((Instruction*)(Inst->pval))->op[i].lexeme, ligne, Symb), 1), *RelocInst);
			else {
				char* tok = ((Instruction*)(Inst->pval))->op[i].lexeme;
				int j=0;
				while(*tok!='('){
					tok++;
					j+=1;
				}
				char* offset;
				offset = strndup(((Instruction*)(Inst->pval))->op[i].lexeme,j);
				*RelocInst=ajouterQueue(symbole_find(Inst, trouverSymbole(offset, ligne, Symb), 1), *RelocInst);
			}
			if(strcmp(((Instruction*)(Inst->pval))->op[i].typeadr,"Imm")==0){
				if(strcmp(((Instruction*)(Inst->pval))->nom,"lui")==0 && ((Instruction*)(Inst->pval))->op[i].categorie==SYMBOLE && (strcmp(((Instruction*)(Inst->suiv->pval))->nom,"lw")==0 || strcmp(((Instruction*)(Inst->suiv->pval))->nom,"sw")==0)){
					int m = trouverSymbole2(((Instruction*)(Inst->pval))->op[i].lexeme,ligne, Symb)->decalage & 0xFFFF0000;
					free(((Instruction*)(Inst->pval))->op[i].lexeme);
					char vale1[250];
					sprintf(vale1,"%d",m);
					((Instruction*)(Inst->pval))->op[i].lexeme=strdup(vale1);	
				}
				else{
				int mot1 = trouverSymbole2(((Instruction*)(Inst->pval))->op[i].lexeme,ligne, Symb)->decalage & 0x0000FFFF;
				free(((Instruction*)(Inst->pval))->op[i].lexeme);
				char val1[250];
				sprintf(val1,"%d",mot1);
				((Instruction*)(Inst->pval))->op[i].lexeme=strdup(val1);
				}
			}
			else if(strcmp(((Instruction*)(Inst->pval))->op[i].typeadr,"Abs")==0){
				int mot2 = trouverSymbole2(((Instruction*)(Inst->pval))->op[i].lexeme,ligne, Symb)->decalage & 0x0FFFFFFF;
				free(((Instruction*)(Inst->pval))->op[i].lexeme);
				char val2[250];
				sprintf(val2,"%d",mot2);
				((Instruction*)(Inst->pval))->op[i].lexeme=strdup(val2);
			}
			else if(strcmp(((Instruction*)(Inst->pval))->op[i].typeadr,"Bas")==0){
				char* to = ((Instruction*)(Inst->pval))->op[i].lexeme;
				int j=0;
				while(*to!='('){
					to++;
					j+=1;
				}
				char* offsets;
				offsets = strndup(((Instruction*)(Inst->pval))->op[i].lexeme,j);
				int mots = trouverSymbole2(offsets,ligne, Symb)->decalage & 0x0000FFFF;
				char* base =strdup(to);
				free(((Instruction*)(Inst->pval))->op[i].lexeme);
				char vals[250];
				sprintf(vals,"%d",mots);
				strcat(vals,base);
				((Instruction*)(Inst->pval))->op[i].lexeme=strdup(vals);
				free(offsets);
				free(base);
				((Instruction*)(Inst->pval))->op[i].categorie=SYMBOLE;
			}
		}
	}
}

void relocationData(ListeG Data,ListeG* Symb,int ligne,ListeG* RelocData){
	int nb=((Donnee1*)(Data->pval))->nbop;
	int i = 0;
	ListeG P=((Donnee1*)(Data->pval))->op->suiv;
	for(i=0; i<nb;i++){
		if(((OpeD*)(P->pval))->type==SYMBOLE){
			*RelocData=ajouterQueue(symbole_find(Data, trouverSymbole(((OpeD*)(P->pval))->valeur.as_et, ligne, Symb), 0), *RelocData);
			int mot1 = trouverSymbole2(((OpeD*)(P->pval))->valeur.as_et, ligne, Symb);
			free(((OpeD*)(P->pval))->valeur.as_et);
			char val1[250];
			sprintf(val1,"%d",mot1);
			((OpeD*)(P->pval))->valeur.as_et=strdup(val1);
		}
		P=P->suiv;
	}
}

Symbole* trouverSymbole(char* nom, int ligne, ListeG* Symb){
	if(listeVide(*Symb)){
		*Symb=ajouterQueue(creerSymbole(nom, SYMBOLE, ligne, UNDEFINED, 0), *Symb);
		return (Symbole*)((*Symb)->suiv->pval);
	}
	else{
		ListeG D=*Symb;
		do{
			if(strcmp(nom,((Symbole*)((D)->suiv->pval))->lexeme)==0)
				return (Symbole*)((D)->suiv->pval);
			D=D->suiv;
		}while (D!=*Symb);
	}
	if(((Symbole*)((*Symb)->pval))->sect==UNDEFINED){
		*Symb=ajouterQueue(creerSymbole(nom, SYMBOLE, ligne, UNDEFINED, (((Symbole*)((*Symb)->pval))->decalage)+4), *Symb);
		return (Symbole*)((*Symb)->pval);
	}
	else{
		*Symb=ajouterQueue(creerSymbole(nom, SYMBOLE, ligne, UNDEFINED, 0), *Symb);
		return (Symbole*)((*Symb)->pval);
	}
}

Symbole* trouverSymbole2(char* nom, int ligne, ListeG* Symb){
	/*printf("mot :%s\n",nom);*/
	if(listeVide(*Symb)){
		ERROR_MSG("Cette étiquette n'est pas définit1 ligne %d",ligne);
	}
	else{
		ListeG D=*Symb;
		do{
			if(strcmp(nom,((Symbole*)((D)->suiv->pval))->lexeme)==0)
				return (Symbole*)((D)->suiv->pval);
			D=D->suiv;
			/*printf("%s\n",((Symbole*)((D)->suiv->pval))->lexeme);*/
		}while (D!=*Symb);
		ERROR_MSG("Cette étiquette n'est pas définit2 ligne %d",ligne);
	}
}

void rel(ListeG* Instruct, ListeG Data, ListeG* Etiquette, ListeG* RelocInst, ListeG* RelocData){
	registres tab[32];
	chargeRegistre(tab);
	if(!listeVide(*Instruct)){
	ListeG A=*Instruct;
	do{
		int vrai=0;/* passe à 1 si l'opérande est une pseudo instruction qui sera remplacé par deux instruction*/
		/*modifie les registre en chiffre*/
		associerReg(A->suiv,tab,((Instruction*)(A->suiv->pval))->ligne);
		/*vérifie que les opérandes sont bon*/
		extraction_des_operandes(((Instruction*)(A->suiv->pval)), *Etiquette);
		/*insertion des pseudo instruction*/
		if(strcmp(((Instruction*)(A->suiv->pval))->nom,"nop")==0 || (strcmp(((Instruction*)(A->suiv->pval))->nom,"lw")==0 && ((Instruction*)(A->suiv->pval))->op[1].categorie==SYMBOLE) || (strcmp(((Instruction*)(A->suiv->pval))->nom,"sw")==0 && ((Instruction*)(A->suiv->pval))->op[1].categorie==SYMBOLE) || strcmp(((Instruction*)(A->suiv->pval))->nom,"neg")==0 || (strcmp(((Instruction*)(A->suiv->pval))->nom,"blt")==0 && ((Instruction*)(A->suiv->pval))->op[2].categorie==SYMBOLE) || strcmp(((Instruction*)(A->suiv->pval))->nom,"move")==0 || strcmp(((Instruction*)(A->suiv->pval))->nom,"li")==0 )/*vérifier les conditions*/{
			if(strcmp(((Instruction*)(A->suiv->pval))->nom,"lw")==0 || strcmp(((Instruction*)(A->suiv->pval))->nom,"sw")==0 || strcmp(((Instruction*)(A->suiv->pval))->nom,"blt")==0)
			vrai=1;
		pseudoInstruction( &(A->suiv));
		}
		relocationInst(A->suiv,Etiquette,((Instruction*)(A->suiv->pval))->ligne, RelocInst);
			if(vrai==1){
				A=A->suiv;
				relocationInst(A->suiv,Etiquette,((Instruction*)(A->suiv->pval))->ligne, RelocInst);
			}
		A=A->suiv;
	}while (A!=*Instruct);
	}
	if(!listeVide(Data)){
	ListeG B=Data;
	do{
		relocationData(B->suiv,Etiquette,((Donnee1*)(B->suiv->pval))->ligne, RelocData);
		B=B->suiv;
	}while (B!=Data);
	}
	libererregistre(tab, 32);
}

void extraction_des_operandes(Instruction* inst, ListeG Symb){
	int i;
	for(i=0;i<inst->nbop;i++){
		switch (inst->op[i].categorie){
			case REGISTRE:
				if(strcmp(inst->op[i].typeadr,"Reg")!=0)
					ERROR_MSG("mauvais type d'opérande pour l'instruction ligne %d",inst->ligne);
				break;
			case DECIMAL:
				if(strcmp(inst->op[i].typeadr,"Imm")==0)
					if((atoi(inst->op[i].lexeme)<-32768) || (atoi(inst->op[i].lexeme)>32767))
						ERROR_MSG("l'opérande sa n'a pas une valeur admissible, ligne %d ",inst->ligne);
				else if(strcmp(inst->op[i].typeadr,"sa")==0)
					if((atoi(inst->op[i].lexeme)<0) || (atoi(inst->op[i].lexeme)>31))
						ERROR_MSG("l'opérande sa n'a pas une valeur admissible ligne %d ",inst->ligne);
				else if(strcmp(inst->op[i].typeadr,"Rel")==0){
					if(strtol(inst->op[i].lexeme,NULL,10)<-131072 || strtol(inst->op[i].lexeme,NULL,10)>131071 || (strtol(inst->op[i].lexeme,NULL,10)%4)!=0)
						ERROR_MSG("l'opérande rel n'a pas une valeur admissible, ligne %d ",inst->ligne);
					free(inst->op[i].lexeme);
					char rel[250];
					sprintf(rel,"%ld",(strtol(inst->op[i].lexeme,NULL,10)>>2));
					inst->op[i].lexeme=strdup(rel);
				}
				else if(strcmp(inst->op[i].typeadr,"Abs")==0){
					if(strtol(inst->op[i].lexeme,NULL,10)<0 || strtol(inst->op[i].lexeme,NULL,10)>268435456 || (strtol(inst->op[i].lexeme,NULL,10)%4)!=0)
						ERROR_MSG("l'opérande abs n'a pas une valeur admissible, ligne %d ",inst->ligne);
					free(inst->op[i].lexeme);
					char abs[250];
					sprintf(abs,"%ld",(strtol(inst->op[i].lexeme,NULL,10)>>2));
					inst->op[i].lexeme=strdup(abs);
				}
				else
					ERROR_MSG("mauvais type d'opérande pour l'instruction ligne %d",inst->ligne);
				break;
			case OCTATE:
				if(strcmp(inst->op[i].typeadr,"Imm")==0)
					if(strtol(inst->op[i].lexeme,NULL,8)<-32768 || strtol(inst->op[i].lexeme,NULL,8)>32767)
						ERROR_MSG("l'opérande sa n'a pas une valeur admissible, ligne %d ",inst->ligne);
				else if(strcmp(inst->op[i].typeadr,"sa")==0)
					if(strtoul(inst->op[i].lexeme,NULL,8)<0 || strtoul(inst->op[i].lexeme,NULL,8)>31)
						ERROR_MSG("l'opérande sa n'a pas une valeur admissible, ligne %d ",inst->ligne);
				else if(strcmp(inst->op[i].typeadr,"Rel")==0){
					if(strtol(inst->op[i].lexeme,NULL,8)<-131072 || strtol(inst->op[i].lexeme,NULL,8)>131071 || (strtol(inst->op[i].lexeme,NULL,8)%4)!=0)
						ERROR_MSG("l'opérande rel n'a pas une valeur admissible, ligne %d ",inst->ligne);
					free(inst->op[i].lexeme);
					char rel[250];
					sprintf(rel,"%ld",(strtol(inst->op[i].lexeme,NULL,8)>>2));
					inst->op[i].lexeme=strdup(rel);
				}
				else if(strcmp(inst->op[i].typeadr,"Abs")==0){
					if(strtol(inst->op[i].lexeme,NULL,8)<0 || strtol(inst->op[i].lexeme,NULL,8)>268435456 || (strtol(inst->op[i].lexeme,NULL,8)%4)!=0)
						ERROR_MSG("l'opérande abs n'a pas une valeur admissible, ligne %d ",inst->ligne);
					free(inst->op[i].lexeme);
					char abs[250];
					sprintf(abs,"%ld",(strtol(inst->op[i].lexeme,NULL,16)>>2));
					inst->op[i].lexeme=strdup(abs);
				}
				else
					ERROR_MSG("mauvais type d'opérande pour l'instruction ligne %d",inst->ligne);
				break;
			case HEXA:
			/*attension passage unsigned long int à unsigned int*/
				if(strcmp(inst->op[i].typeadr,"Imm")==0)
					if(strtol(inst->op[i].lexeme,NULL,16)<-32768 || strtol(inst->op[i].lexeme,NULL,16)>32767)
						ERROR_MSG("l'opérande imm n'a pas une valeur admissible, ligne %d ",inst->ligne);
				else if(strcmp(inst->op[i].typeadr,"sa")==0)
					if(strtoul(inst->op[i].lexeme,NULL,16)<0 || strtoul(inst->op[i].lexeme,NULL,16)>31)
						ERROR_MSG("l'opérande sa n'a pas une valeur admissible, ligne %d ",inst->ligne);
				else if(strcmp(inst->op[i].typeadr,"Rel")==0){
					if(strtol(inst->op[i].lexeme,NULL,16)<-131072 || strtol(inst->op[i].lexeme,NULL,16)>131071 || (strtol(inst->op[i].lexeme,NULL,16)%4)!=0)
						ERROR_MSG("l'opérande rel n'a pas une valeur admissible, ligne %d ",inst->ligne);
					free(inst->op[i].lexeme);
					char rel[250];
					sprintf(rel,"%ld",(strtol(inst->op[i].lexeme,NULL,16)>>2));
					inst->op[i].lexeme=strdup(rel);
				}
				else if(strcmp(inst->op[i].typeadr,"Abs")==0){
					if(strtol(inst->op[i].lexeme,NULL,16)<0 || strtol(inst->op[i].lexeme,NULL,16)>268435456 || (strtol(inst->op[i].lexeme, NULL,16)%4)!=0)
						ERROR_MSG("l'opérande abs n'a pas une valeur admissible, ligne %d ",inst->ligne);
					free(inst->op[i].lexeme);
					char abs[250];
					sprintf(abs,"%ld",(strtol(inst->op[i].lexeme,NULL,16)>>2));
					inst->op[i].lexeme=strdup(abs);
				}
				else
					ERROR_MSG("mauvais type d'opérande pour l'instruction ligne %d",inst->ligne);
				break;
			case BASE_OF:
				if(strcmp(inst->op[i].typeadr,"Bas")==0){
					char* tok1 = inst->op[i].lexeme;
					int j=0;
					while(*tok1!='('){
						tok1++;
						j+=1;
					}
					char* offs;
					offs = strndup(inst->op[i].lexeme,j);
					if(strtol(offs,NULL,0)<-32768 || strtol(offs,NULL,0)>32767)
						ERROR_MSG("l'opérande imm n'a pas une valeur admissible, ligne %d ",inst->ligne);
				}
				else
					ERROR_MSG("mauvais type d'opérande pour l'instruction ligne %d",inst->ligne);
				break;
			case SYMBOLE:
				if((strcmp(inst->op[i].typeadr,"Imm")==0) || (strcmp(inst->op[i].typeadr,"Abs")==0) || (strcmp(inst->op[i].typeadr,"Bas")==0));/*ces cas sont pour la relocation, les seul deux Bas correspondes aux pseudo instruction -> reloc*/
				else if(strcmp(inst->op[i].typeadr,"Rel")==0){
					Symbole* pointeur;
					pointeur = trouverSymbole2(inst->op[i].lexeme, inst->ligne, &Symb);
					/*printf("ok+++++++++++++++++++++++++++++++++++++");*/
					if(pointeur->sect==TEXT){
						int saut=(pointeur->decalage)-(inst->decalage);
						free(inst->op[i].lexeme);
						char nouv[250];
						sprintf(nouv,"%d",saut);
						inst->op[i].lexeme=strdup(nouv);
					}
					else
						ERROR_MSG("symbole pas dans la section text pour l'instruction ligne %d",inst->ligne);
				}
				else/*erreur pour les sa, reg et bas*/
					ERROR_MSG("mauvais type d'opérande pour l'instruction %d",inst->ligne);
				break;
			default:
				ERROR_MSG("mauvais type d'opérande pour l'instruction %d",inst->ligne);
		}

	}
}
