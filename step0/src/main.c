
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



    /* ---------------- do the lexical analysis -------------------*/
	File F=creerFile();
    lex_load_file( file, &nlines,&F );

	File P=F;
	do{
		afficherFile(P);
		P=P->suiv;
	}while (P!=F);
	
	/* ---------------- do the gramatical analysis -------------------*/
	ListeG Inst=NULL;
	ListeG Symb=NULL;
	ListeG Do1=NULL;
	ListeG Do2=NULL;

	gramAnalyse(F, &Inst, &Symb, &Do1, &Do2);
	ListeG A=Inst;
	do{
		afficherInst((Instruction*)(A->suiv->pval) );
		A=A->suiv;
	}while (A!=Inst);
printf("\nAffichage section DATA\n");
	ListeG B=Do1;
	do{
		afficherInst((Donnee1*)(B->suiv->pval) );
		B=B->suiv;
	}while (B!=Do1);
    DEBUG_MSG("source code got %d lines",nlines);

    /* ---------------- Free memory and terminate -------------------*/

    /* TODO free everything properly*/

    exit( EXIT_SUCCESS );
}

