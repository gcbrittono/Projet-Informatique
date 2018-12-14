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
#include <pelf/pelf.h>
#include <pelf/section.h>


/*Fonction pour stocker les donnes liées aux table de relocation*/

void relocation_text_elf(ListeG RelocInst, section* reltext, section symtab, section shstrtab, section strtab){
	int nombre_tables = nombres_symboles (RelocInst);
	Elf32_Rel text_reloc[nombre_tables];
	int i;

	for (0; i < nombre_tables; i++){
		text_reloc[i].r_offset =((table_relocation*)(RelocInst->pval))->addr_relative;
		text_reloc[i].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab, strtab,strdup(((table_relocation*)(RelocInst->pval))->sect)),((table_relocation*)(RelocInst->pval))->mode_relocation);
	}
	reltext  = make_rel32_section( ".rel.text", text_reloc, nombre_tables);

}


/*Fonction pour stocker les donnes liées aux table de relocation*/
void relocation_data_elf(ListeG RelocData, section* reldata, section symtab, section shstrtab, section strtab){
	int nombre_tables = nombres_symboles (RelocData);
	Elf32_Rel data_reloc[nombre_tables];
	int i;
	for (i = 0; i < nombre_tables; i++){
	data_reloc[i].r_offset =((table_relocation*)(RelocData->pval))->addr_relative;
	data_reloc[i].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab,strtab,((table_relocation*)(RelocData->pval))->sect),((table_relocation*)(RelocData->pval))->mode_relocation);
	}	
	
	reldata  = make_rel32_section( ".rel.data", data_reloc,nombre_tables);
}





/*Fonction qui stocke les symboles de la liste generique dans un tableu de type Symbole*/
Elf32_Sym* stockages_symboles (ListeG* listeSymboles, section strtab, section shstrtab, char* sym_char[]){
	int i = 0;	
	int quantite_symboles = nombres_symboles (listeSymboles);
	Elf32_Sym syms[quantite_symboles];
	ListeG D = listeSymboles;
		do{
			syms[i].st_name = elf_get_string_offset( strtab->start, strtab->sz, sym_char[i] );
			syms[i].st_size = 0;
			syms[i].st_value = ((Symbole*)(D->pval))->decalage;
			syms[i].st_info = ELF32_ST_INFO( STB_LOCAL, STT_NOTYPE );
			syms[i].st_other = 0;
			syms[i].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, strdup(((Symbole*)(D->pval))->sect) );

			D=D->suiv;
			i++;
		}while (D!=listeSymboles);			

	return syms;
}





/*fonction qui prendre un tableu des binaires provenant des sections .text ou . data et qui les ajoute aux sections text_prog data_prog*/
void affichage_donnees_sections (int* sect_prog, int* tableu_binaires) {
	int nombre_lignes = sizeof(tableu_binaires) / sizeof(int);
	int i;
	for(i=0; i<nombre_lignes; i++){
		sect_prog[0] = tableu_binaires[nombre_lignes];
	}
}

/*Fonction qui donne les nombres des symboles dans la liste generiques*/
int nombres_symboles (ListeG* listeSymboles) {
		int i = 0;
		ListeG D=listeSymboles;
		do{
			D=D->suiv;
			i++;
		}while (D!=listeSymboles);
		return i;
}


/*Fonction qui met les symboles contenu dans la liste generique ListeSymboles dans le tableu des symboles sym_char*/
void affichage_donnees_symboles (char* sym_char[], ListeG* listeSymboles) {
		int i = 0;
		ListeG D=listeSymboles;
		do{
			sym_char[i] = strdup(((Symbole*)(D->pval))->lexeme);
			D=D->suiv;
			i++;
		}while (D!=listeSymboles);
}


/* Fonction general pour la generation des fichiers elf*/
void afichage_sections_elf(int* binaires_text, int* binaires_data, ListeG* listeSymboles, ListeG RelocInst, ListeG RelocData){
	/*relever des tailles des tableus*/
	int nombre_ligne_text = sizeof(binaires_text)/sizeof(int);
	int nombre_ligne_data = sizeof(binaires_data)/sizeof(int);	
	int nombre_symboles   = nombres_symboles(listeSymboles);

	char * sym_char[];

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

	/*Affichage des donnés*/
	affichage_donnees_sections (text_prog, binaires_text);
	affichage_donnees_sections (data_prog, binaires_data);
	affichage_donnees_symboles(sym_char, listeSymboles);



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

	

	Elf32_Sym  syms = stockages_symboles (listeSymboles, strtab, shstrtab, sym_char[]);

	symtab   = make_symtab_section( shstrtab, strtab, syms,nombre_symboles);





	reltext  = relocation_text_elf(RelocInst, reltext);
	reldata = relocation_data_elf(RelocData, reldata);	










	 
	


    /*write these sections in file*/
    elf_write_relocatable( name, machine, noreorder,
                           text->start, text->sz,
                           data->start, data->sz,
                           bss->start, bss->sz,
                           shstrtab->start, shstrtab->sz,
                           strtab->start, strtab->sz,
                           symtab->start, symtab->sz,
                           reltext->start, reltext->sz,
                           reldata->start, reldata->sz);


    print_section( text );
    print_section( data );
    print_section( bss );
    print_section( strtab );
    print_section( symtab );


    /*clean up */
    del_section(     text );
    del_section(     data );
    del_section(      bss );
    del_section( shstrtab );
    del_section(   strtab );
    del_section(   symtab );
    del_section(  reltext );
    del_section(  reldata );

    return 0;







}
