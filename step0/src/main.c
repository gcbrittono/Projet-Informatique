
/**
 * @file main.c
 * @author François Portet <francois.portet@imag.fr> from François Cayre
 * @brief Main entry point for MIPS assembler.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#include <global.h>
#include <notify.h>
#include <lex.h>
#include <gram.h>
#include <rel.h>
#include <gen.h>

/**
 * @param exec Name of executable.
 * @return Nothing.
 * @brief Print usage.
 *
 */
void print_usage( char *exec ) {
    fprintf(stderr, "Usage: %s file.s\n",
            exec);
}



/**
 * @param argc Number of arguments on the command line.
 * @param argv Value of arguments on the command line.
 * @return Whether this was a success or not.
 * @brief Main entry point for MIPS assembler.
 *
 */
int main ( int argc, char *argv[] ) {

    unsigned int 	nlines 	= 0;
    char         	 *file 	= NULL;

    /* exemples d'utilisation des macros du fichier notify.h */
    /* WARNING_MSG : sera toujours affiche */
    WARNING_MSG("Un message WARNING_MSG !");

    /* macro INFO_MSG : uniquement si compilé avec -DVERBOSE. Cf. Makefile*/
    INFO_MSG("Un message INFO_MSG : Debut du programme %s", argv[0]);

    /* macro DEBUG_MSG : uniquement si compilé avec -DDEBUG (ie : compilation avec make debug). Cf. Makefile */
    DEBUG_MSG("Un message DEBUG_MSG !");

    /* La macro suivante provoquerait l'affichage du message
       puis la sortie du programme avec un code erreur non nul (EXIT_FAILURE) */
    /* ERROR_MSG("Erreur. Arret du programme"); */


    if ( argc <2 ) {
        print_usage(argv[0]);
        exit( EXIT_FAILURE );
    }


    file  	= argv[argc-1];


    if ( NULL == file ) {
        fprintf( stderr, "Missing ASM source file, aborting.\n" );
        exit( EXIT_FAILURE );
    }

/*------------------------affichage---------------------------------*/

printf("voulez-vous afficher le contenu des listes? (oui : 1 / non : 0)\n");
int reponse=-1;
while(reponse!=0 && reponse!=1)
	scanf("%d", &reponse);


/* ---------------- do the lexical analysis -------------------*/
	int erreur=0;
	File F=creerFile();
	int tailletext=0;
	int tailledata=0;
    lex_load_file( file, &nlines,&F, &erreur);

	File P=F;
	do{
		if(reponse==1)
			afficherFile(P);
		P=P->suiv;
	}while (P!=F);
	if (erreur == 1)
		ERROR_MSG("Erreur dans l'annalyse lexicale");
/* ---------------- do the gramatical analysis -------------------*/
	ListeG Inst=NULL;
	ListeG Symb=NULL;
	ListeG Do1=NULL;
	ListeG Do2=NULL;

	gramAnalyse(F, &Inst, &Symb, &Do1, &Do2,&erreur);

	if (erreur == 1)
		ERROR_MSG("Erreur dans l'annalyse grammaticale");

/* ---------------- Affichage données data -------------------*/

	if(listeVide(Do1))
		if(reponse==1)
			printf("La section DATA est vide\n");
	else{
		printf("\nAffichage section DATA\n");
		ListeG B=Do1;
		do{
			tailledata+=((Donnee1*)(B->suiv->pval))->nbop;
			if(reponse==1)
				afficherDo1((Donnee1*)(B->suiv->pval) );
			B=B->suiv;
		}while (B!=Do1);
	}
/* ---------------- Affichage données bss-------------------*/

	if(reponse==1){
		if(listeVide(Do2))
			printf("La section BSS est vide\n");
		else{
			printf("\nAffichage section BSS\n");
			ListeG C=Do2;
			do{
					afficherDo2((Donnee2*)(C->suiv->pval) );
				C=C->suiv;
			}while (C!=Do2);
		}
	}
/* ---------------- Affichage etiquettes-------------------*/
	
	if(reponse==1){
		if(listeVide(Symb))
			printf("La section SYMBOLE est vide\n");
		else{
			printf("\nAffichage section SYMBOLE\n");
			ListeG D=Symb;
			do{
					afficherSymb((Symbole*)(D->suiv->pval) );
				D=D->suiv;
			}while (D!=Symb);
		}
	}	
	if(reponse==1){
		if(listeVide(Inst))
			printf("la section TEXT est vide\n");
		else{
			printf("\nAffichage section TEXT avant relocation\n");
			ListeG A=Inst;
			do{
					afficherInst((Instruction*)(A->suiv->pval) );
				A=A->suiv;
			}while (A!=Inst);
		}
	}
/*---------------relocation---------------------------------*/
ListeG RelocInst=NULL;
ListeG RelocData=NULL;
rel(&Inst, Do1, &Symb, &RelocInst, &RelocData);

	
	if(listeVide(Inst))
		printf("la section TEXT est vide après relocation");
	else{
		if(reponse==1)
			printf("\nAffichage section TEXT après relocation\n");
		ListeG A=Inst;
		do{
			tailletext+=1;
			if(reponse==1)
				afficherInst((Instruction*)(A->suiv->pval) );
			A=A->suiv;
		}while (A!=Inst);
	}
	if(reponse==1){
		if(listeVide(Symb))
			printf("la section SYMBOLE est vide aprè relocation\n");
		else{
			printf("\nAffichage section SYMBOLE après relocation\n");
			ListeG D=Symb;
			do{
					afficherSymb((Symbole*)(D->suiv->pval) );
				D=D->suiv;
			}while (D!=Symb);
		}
	}

	if(reponse==1){
		if(listeVide(RelocInst))
			printf("la section de relocation text est vide\n");
		else{
			printf("\nAffichage section relocation text après relocation\n");
			ListeG E=RelocInst->suiv;
			do{
					printf("%s - mode : %d - decalage : %d -\n",((table_relocation*)(E->pval))->pointeur->lexeme, ((table_relocation*)(E->pval))->mode_relocation,((table_relocation*)(E->pval))->addr_relative);
				E=E->suiv;
			}while (E!=RelocInst->suiv);
		}
	}
printf("\nAffichage section relocation data après relocation\n");
	if(listeVide(RelocData))
		printf("la section de relocation data est vide\n");
	else{
		ListeG F=RelocData->suiv;
		do{
			if(reponse==1)
				printf("%s - mode : %d - décalage : %d -\n",((table_relocation*)(F->pval))->pointeur->lexeme,((table_relocation*)(F->pval))->mode_relocation,((table_relocation*)(F->pval))->addr_relative);
			F=F->suiv;
		}while (F!=RelocData->suiv);
	}
/*--------------------------Generation----------------------------------*/
printf("\ngénération du binaire\n");
unsigned int texttab[tailletext];
unsigned int datatab[tailledata];
dico_bin tab[25];
chargeDico(tab,25);
gen(Inst, Do1, tab, 25,tailletext,tailledata,texttab,datatab);
/*--------------------------------------------------------------*/
    DEBUG_MSG("source code got %d lines",nlines);
/* ---------------- Free memory and terminate -------------------*/
	libererInstruction(&Inst);

	libererSymbole(&Symb);
	libererDo2(&Do2);
	libererDo1(&Do1);
	libererFile(&F);
    /* TODO free everything properly*/

    exit( EXIT_SUCCESS );
}
