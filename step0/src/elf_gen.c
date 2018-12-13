#include <stdlib.h>
#include <string.h>

#include <pelf/pelf.h>
#include <pelf/section.h>





void affichage_donnees_sections (int* sect_prog, int* tableu_binaires) {
	int nombre_lignes = sizeof(tableu_binaires) / sizeof(int);
	int i;
	for(i=0; i<nombre_lignes; i++){
		sect_prog[0] = tableu_binaires[nombre_lignes];
	}
}


int nombres_symboles (ListeG* listeSymboles) {
		int i = 0;
		ListeG D=listeSymboles;
		do{
			sym_char[i] = strdup(((Symbole*)(D->pval->lexeme)));
			D=D->suiv;
			i++;
		}while (D!=listeSymboles);
		return i;
}



void affichage_donnees_symboles (char* sym_char[], ListeG* listeSymboles) {
		int i = 0;
		ListeG D=listeSymboles;
		do{
			sym_char[i] = strdup(((Symbole*)(D->pval->lexeme)));
			D=D->suiv;
			i++;
		}while (D!=listeSymboles);
}



void afichage_sections_elf(int* binaires_text, int* binaires_data, ListeG* listeSymboles){
	
	nombre_ligne_text = sizeof(binaires_text)/sizeof(int);
	nombre_ligne_data = sizeof(binaires_data)/sizeof(int);	
	nombre_symboles   = nombres_symboles(listeSymboles);

	/* prepare sections*/
	section     text = NULL;
	section     data = NULL;
	section      bss = NULL;
	section shstrtab = NULL;
	section   strtab = NULL;
	section   symtab = NULL;
	section  reltext = NULL;
	section  reldata = NULL;

	int text_prog[nombre_ligne_text];
	int data_prog[nombre_ligne_data];
	char sym_char[nombre_symboles];



	char* machine = "mips";
	char* name = "exemple.o";
	/* pelf options */
	int noreorder =1;



	/* make predefined section table*/
	shstrtab = make_shstrtab_section();

	
	affichage_donnees_sections (text_prog, int* tableu_binaires);
	affichage_donnees_sections (data_prog, int* tableu_binaires);*
	affichage_donnees_symboles(sym_char[], listeSymboles);



	text     = make_text_section(text_prog, nombre_ligne_text);
	if ( !text ) {
		fprintf( stderr, "Unable to write .text section (missing information).\n" );
		return -1;
	}

	data = make_data_section(  data_prog, nombre_ligne_data);
	if ( !data ) {
		fprintf( stderr, "Unable to write .data section (missing information).\n" );
		return -1;
	}

	bss = make_bss_section(  bss_prog);
	if ( !bss ) {
		fprintf( stderr, "Unable to write .bss section (missing information).\n" );
		return -1;
	}

	strtab   = make_strtab_section( sym_char, nombre_symboles);

}
