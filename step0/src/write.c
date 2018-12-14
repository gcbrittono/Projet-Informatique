
/**
 * @file write.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Sat Sep 23 01:35:48 2017
 * @brief Write ELF.
 *
 * Write ELF.
 */

#include <pelf/pelf.h>



#define ELF_RELOCATABLE_NSECTIONS 9

static int elf_write_relocatable_preamble( FILE *fp, unsigned int machine, unsigned int shnum, unsigned int shstrndx, int noreorder ) {
    Elf_Ehdr ehdr;
    PELF_DECLARE2( Ehdr, header, &ehdr );

    switch ( elf_get_machine_class( machine ) ) {
    case ELFCLASS32 :
        header32->e_ident[EI_MAG0   ] = ELF_MAG0;
        header32->e_ident[EI_MAG1   ] = ELF_MAG1;
        header32->e_ident[EI_MAG2   ] = ELF_MAG2;
        header32->e_ident[EI_MAG3   ] = ELF_MAG3;
        header32->e_ident[EI_CLASS  ] = elf_get_machine_class( machine );
        header32->e_ident[EI_DATA   ] = elf_get_machine_endianness( machine );
        header32->e_ident[EI_VERSION] = EV_CURRENT;
        header32->e_ident[EI_PAD    ] = '\0';

        header32->e_type              = ET_REL;
        header32->e_machine           = machine;
        header32->e_version           = EV_CURRENT;
        header32->e_entry             = 0;
        header32->e_phoff             = 0;
        header32->e_shoff             = elf_get_machine_ehsize( machine ); /* Write SHT right after the header */
        header32->e_flags             = EF_MIPS_PIC+noreorder;
        header32->e_ehsize            = elf_get_machine_ehsize( machine );
        header32->e_phentsize         = elf_get_machine_phentsize( machine );
        header32->e_phnum             = 0;
        header32->e_shentsize         = elf_get_machine_shentsize( machine );
        header32->e_shnum             = shnum;
        header32->e_shstrndx          = shstrndx;

        if ( PELF_HOST_ENDIANNESS != elf_get_machine_endianness( machine ) ) {
            elf32_endian_flip_Ehdr( header32 );
        }

        return sizeof(*header32) != fwrite( header32, 1, sizeof(*header32), fp );

    case ELFCLASS64 :
        header64->e_ident[EI_MAG0   ] = ELF_MAG0;
        header64->e_ident[EI_MAG1   ] = ELF_MAG1;
        header64->e_ident[EI_MAG2   ] = ELF_MAG2;
        header64->e_ident[EI_MAG3   ] = ELF_MAG3;
        header64->e_ident[EI_CLASS  ] = elf_get_machine_class( machine );
        header64->e_ident[EI_DATA   ] = elf_get_machine_endianness( machine );
        header64->e_ident[EI_VERSION] = EV_CURRENT;
        header64->e_ident[EI_PAD    ] = '\0';

        header64->e_type              = ET_REL;
        header64->e_machine           = machine;
        header64->e_version           = EV_CURRENT;
        header64->e_entry             = 0;
        header64->e_phoff             = 0;
        header64->e_shoff             = elf_get_machine_ehsize( machine ); /* Write SHT right after the header */
        header64->e_flags             = EF_MIPS_PIC+noreorder;
        header64->e_ehsize            = elf_get_machine_ehsize( machine );
        header64->e_phentsize         = elf_get_machine_phentsize( machine );
        header64->e_phnum             = 0;
        header64->e_shentsize         = elf_get_machine_shentsize( machine );
        header64->e_shnum             = shnum;
        header64->e_shstrndx          = shstrndx;

        if ( PELF_HOST_ENDIANNESS != elf_get_machine_endianness( machine ) ) {
            elf64_endian_flip_Ehdr( header64 );
        }

        return sizeof(*header64) != fwrite( header64, 1, sizeof(*header64), fp );

    default :
        return 0;
    }

}

#define WITH_SECTION(name, data)					\
  section = name;							\
  ptr     = data;							\
  size    = data##sz;							\
  if ( ptr ) {								\
    shnum                    = elf_get_string_index( shstrtab,		\
						     shstrtabsz,	\
						     section );		\
    shdr[shnum].sh_name      = elf_get_string_offset( shstrtab,		\
						      shstrtabsz,	\
						      section );	\
    shdr[shnum].sh_addr      = 0;					\
    shdr[shnum].sh_offset    = shdr[prev_shnum].sh_offset +		\
      shdr[prev_shnum].sh_size;						\
    shdr[shnum].sh_size      = size


#define END_SECTION							\
  prev_shnum               = shnum;					\
  if ( shdr[shnum].sh_size ) {						\
  if ( shdr[shnum].sh_size						\
       !=								\
       fwrite( ptr, 1, shdr[shnum].sh_size, fp ) ) {			\
    fprintf( stderr, "Unable to write section %s.\n", section );	\
    return -1;								\
  }									\
  }									\
  }

static int elf32_write_sections( FILE *fp,
                                 int   endianness,
                                 unsigned char *text, unsigned int textsz,
                                 unsigned char *data, unsigned int datasz,
                                 unsigned char *bss, unsigned int bsssz,
                                 unsigned char *shstrtab, unsigned int shstrtabsz,
                                 unsigned char *strtab, unsigned int strtabsz,
                                 unsigned char *symtab, unsigned int symtabsz,
                                 unsigned char *reltext, unsigned int reltextsz,
                                 unsigned char *reldata, unsigned int reldatasz) {

    Elf32_Shdr          shdr[1+ELF_RELOCATABLE_NSECTIONS];
    size_t          filepos = ftell( fp );
    int               shnum = 0;
    int          prev_shnum = 0;
    char           *section = NULL;
    unsigned char      *ptr = NULL;
    size_t             size = 0;
    unsigned int          i;

    /* first section entry: special SHN_UNDEF */

    fseek( fp, filepos+(1+ELF_RELOCATABLE_NSECTIONS)*sizeof( shdr[0] ), SEEK_SET );

    shdr[shnum].sh_name      = 0;
    shdr[shnum].sh_type      = SHT_NULL;
    shdr[shnum].sh_flags     = 0;
    shdr[shnum].sh_addr      = 0;
    shdr[shnum].sh_offset    = filepos+(1+ELF_RELOCATABLE_NSECTIONS)*sizeof( shdr[0] );
    shdr[shnum].sh_size      = 0;
    shdr[shnum].sh_link      = SHN_UNDEF;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;


    /* .text section */
    WITH_SECTION( ".text", text );
    shdr[shnum].sh_type      = SHT_PROGBITS;
    shdr[shnum].sh_flags     = SHF_ALLOC+SHF_EXECINSTR;
    shdr[shnum].sh_link      = SHN_UNDEF;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;
    END_SECTION;

    /* .rel.text section */
    WITH_SECTION( ".rel.text", reltext );
    shdr[shnum].sh_type      = SHT_REL;
    shdr[shnum].sh_flags     = 0;
    shdr[shnum].sh_link      = elf_get_string_index( shstrtab, shstrtabsz, ".symtab" );
    shdr[shnum].sh_info      = elf_get_string_index( shstrtab, shstrtabsz, ".text" );
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = sizeof( Elf32_Rel );
    END_SECTION;


    /* .data section */
    WITH_SECTION( ".data", data ) ;
    shdr[shnum].sh_type      = SHT_PROGBITS;
    shdr[shnum].sh_flags     = SHF_ALLOC+SHF_WRITE;
    shdr[shnum].sh_link      = SHN_UNDEF;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;
    END_SECTION;

    /* .rel.data section */
    WITH_SECTION( ".rel.data", reldata );
    shdr[shnum].sh_type      = SHT_REL;
    shdr[shnum].sh_flags     = 0;
    shdr[shnum].sh_link      = elf_get_string_index( shstrtab, shstrtabsz, ".symtab" );
    shdr[shnum].sh_info      = elf_get_string_index( shstrtab, shstrtabsz, ".data" );
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = sizeof( Elf32_Rel );
    END_SECTION;

    /* .bss section */
    WITH_SECTION( ".bss", bss );
    shdr[shnum].sh_type      = SHT_NOBITS;
    shdr[shnum].sh_flags     = SHF_ALLOC+SHF_WRITE;
    shdr[shnum].sh_link      = SHN_UNDEF;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;
    END_SECTION;

    /* .symtab section */
    /* check ELF_Format page 19 www.skyfree.org/linux/references/ELF_Format.pdf
    sh_info should contains One greater than the symbol table index of the last local symbol (binding STB_LOCAL).*/
    WITH_SECTION( ".symtab", symtab );
    shdr[shnum].sh_type      = SHT_SYMTAB;
    shdr[shnum].sh_flags     = SHF_ALLOC;
    shdr[shnum].sh_link      = elf_get_string_index( shstrtab, shstrtabsz, ".strtab" );
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = sizeof( Elf32_Sym );
    END_SECTION;

    /* .strtab section */
    WITH_SECTION( ".strtab", strtab );
    shdr[shnum].sh_type      = SHT_STRTAB;
    shdr[shnum].sh_flags     = SHF_ALLOC;
    shdr[shnum].sh_link      = 0;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;
    END_SECTION;

    /* .shstrtab section */
    WITH_SECTION( ".shstrtab", shstrtab );
    shdr[shnum].sh_type      = SHT_STRTAB;
    shdr[shnum].sh_flags     = 0;
    shdr[shnum].sh_link      = 0;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;
    END_SECTION;


    /* Restore */
    shdr[0].sh_offset        = 0;

    if ( PELF_HOST_ENDIANNESS != endianness ) {
        for ( i = 0; i< ELF_RELOCATABLE_NSECTIONS; i++ ) {
            elf32_endian_flip_Shdr( &(shdr[i]) );
        }
    }

    fseek( fp, filepos, SEEK_SET );
    if ( (1+ELF_RELOCATABLE_NSECTIONS) * sizeof( shdr[0] )
            !=
            fwrite( &(shdr[0]), 1, (1+ELF_RELOCATABLE_NSECTIONS)*sizeof( shdr[0] ), fp ) ) {
        return -1;
    }

    return ELF_RELOCATABLE_NSECTIONS * sizeof( shdr[0] );
}

static int elf64_write_sections( FILE *fp,
                                 int   endianness,
                                 unsigned char *text, unsigned int textsz,
                                 unsigned char *data, unsigned int datasz,
                                 unsigned char *bss, unsigned int bsssz,
                                 unsigned char *shstrtab, unsigned int shstrtabsz,
                                 unsigned char *strtab, unsigned int strtabsz,
                                 unsigned char *symtab, unsigned int symtabsz,
                                 unsigned char *reltext, unsigned int reltextsz,
                                 unsigned char *reldata, unsigned int reldatasz ) {

    Elf64_Shdr          shdr[1+ELF_RELOCATABLE_NSECTIONS];
    size_t          filepos = ftell( fp );
    int               shnum = 0;
    int          prev_shnum = 0;
    char           *section = NULL;
    unsigned char      *ptr = NULL;
    size_t             size = 0;
    unsigned int          i;

    /* first section entry: special SHN_UNDEF */

    fseek( fp, filepos+(1+ELF_RELOCATABLE_NSECTIONS)*sizeof( shdr[0] ), SEEK_SET );

    shdr[shnum].sh_name      = 0;
    shdr[shnum].sh_type      = SHT_NULL;
    shdr[shnum].sh_flags     = 0;
    shdr[shnum].sh_addr      = 0;
    shdr[shnum].sh_offset    = filepos+(1+ELF_RELOCATABLE_NSECTIONS)*sizeof( shdr[0] );
    shdr[shnum].sh_size      = 0;
    shdr[shnum].sh_link      = SHN_UNDEF;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;


    /* .text section */
    WITH_SECTION( ".text", text );
    shdr[shnum].sh_type      = SHT_PROGBITS;
    shdr[shnum].sh_flags     = SHF_ALLOC+SHF_EXECINSTR;
    shdr[shnum].sh_link      = SHN_UNDEF;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;
    END_SECTION;

    /* .rel.text section */
    WITH_SECTION( ".rel.text", reltext );
    shdr[shnum].sh_type      = SHT_REL;
    shdr[shnum].sh_flags     = 0;
    shdr[shnum].sh_link      = elf_get_string_index( shstrtab, shstrtabsz, ".symtab" );
    shdr[shnum].sh_info      = elf_get_string_index( shstrtab, shstrtabsz, ".text" );
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = sizeof( Elf64_Rel );
    END_SECTION;

    /* .data section */
    WITH_SECTION( ".data", data ) ;
    shdr[shnum].sh_type      = SHT_PROGBITS;
    shdr[shnum].sh_flags     = SHF_ALLOC+SHF_WRITE;
    shdr[shnum].sh_link      = SHN_UNDEF;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;
    END_SECTION;

    /* .rel.data section */
    WITH_SECTION( ".rel.data", reldata );
    shdr[shnum].sh_type      = SHT_REL;
    shdr[shnum].sh_flags     = 0;
    shdr[shnum].sh_link      = elf_get_string_index( shstrtab, shstrtabsz, ".symtab" );
    shdr[shnum].sh_info      = elf_get_string_index( shstrtab, shstrtabsz, ".data" );
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = sizeof( Elf64_Rel );
    END_SECTION;

    /* .bss section */
    WITH_SECTION( ".bss", bss );
    shdr[shnum].sh_type      = SHT_NOBITS;
    shdr[shnum].sh_flags     = SHF_ALLOC+SHF_WRITE;
    shdr[shnum].sh_link      = SHN_UNDEF;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;
    END_SECTION;

    /* .symtab section */
    WITH_SECTION( ".symtab", symtab );
    shdr[shnum].sh_type      = SHT_SYMTAB;
    shdr[shnum].sh_flags     = SHF_ALLOC;
    shdr[shnum].sh_link      = elf_get_string_index( shstrtab, shstrtabsz, ".strtab" );
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = sizeof( Elf64_Sym );
    END_SECTION;

    /* .strtab section */
    WITH_SECTION( ".strtab", strtab );
    shdr[shnum].sh_type      = SHT_STRTAB;
    shdr[shnum].sh_flags     = SHF_ALLOC;
    shdr[shnum].sh_link      = 0;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;
    END_SECTION;

    /* .shstrtab section */
    WITH_SECTION( ".shstrtab", shstrtab );
    shdr[shnum].sh_type      = SHT_STRTAB;
    shdr[shnum].sh_flags     = 0;
    shdr[shnum].sh_link      = 0;
    shdr[shnum].sh_info      = 0;
    shdr[shnum].sh_addralign = 0;
    shdr[shnum].sh_entsize   = 0;
    END_SECTION;


    /* Restore */
    shdr[0].sh_offset        = 0;

    if ( PELF_HOST_ENDIANNESS != endianness ) {
        for ( i = 0; i< ELF_RELOCATABLE_NSECTIONS; i++ ) {
            elf64_endian_flip_Shdr( &(shdr[i]) );
        }
    }

    fseek( fp, filepos, SEEK_SET );
    if ( (1+ELF_RELOCATABLE_NSECTIONS) * sizeof( shdr[0] )
            !=
            fwrite( &(shdr[0]), 1, (1+ELF_RELOCATABLE_NSECTIONS)*sizeof( shdr[0] ), fp ) ) {
        return -1;
    }

    return ELF_RELOCATABLE_NSECTIONS * sizeof( shdr[0] );
}

int   elf_write_relocatable( char *filename, char *machinename, int noreorder,
                             unsigned char *text, unsigned int textsz,
                             unsigned char *data, unsigned int datasz,
                             unsigned char *bss, unsigned int bsssz,
                             unsigned char *shstrtab, unsigned int shstrtabsz,
                             unsigned char *strtab, unsigned int strtabsz,
                             unsigned char *symtab, unsigned int symtabsz,
                             unsigned char *reltext, unsigned int reltextsz,
                             unsigned char *reldata, unsigned int reldatasz) {

    int      machine = elf_get_machine( machinename );
    int        width = elf_get_machine_class( machine );
    int   endianness = elf_get_machine_endianness( machine );
    FILE         *fp = NULL;

    if ( -1 == machine ) return -1;

    fp = fopen( filename, "w" );

    if ( !fp ) {
        fprintf( stderr, "Unable to open '%s' for writing.\n", filename );
        return -1;
    }

    if ( -1 == elf_write_relocatable_preamble( fp, machine,
            ELF_RELOCATABLE_NSECTIONS,
            elf_get_string_index( shstrtab,
                                  shstrtabsz,
                                  ".shstrtab" ),
            noreorder ) ) {
        fclose( fp );
        return -1;
    }


    if ( PELF_HOST_ENDIANNESS != endianness ) {
        PELF_DECLARE2( Sym, sym, symtab );
        PELF_DECLARE2( Rel, rel, reltext );

        unsigned int       i = 0;

        for ( i = 0; i< symtabsz/PELF_SIZEOF( width, Sym ); PELF_NEXT2(sym), i++ ) {
            PELF_FLIP2( width, Sym, sym );
        }

        rel32 = (Elf32_Rel*)reltext, rel64 = (Elf64_Rel*)reltext;
        for ( i = 0; i< reltextsz/PELF_SIZEOF( width, Rel ); PELF_NEXT2(rel), i++ ) {
            PELF_FLIP2( width, Rel, rel );
        }

        rel32 = (Elf32_Rel*)reldata, rel64 = (Elf64_Rel*)reldata;
        for ( i = 0; i< reldatasz/PELF_SIZEOF( width, Rel ); PELF_NEXT2(rel), i++ ) {
            PELF_FLIP2( width, Rel, rel );
        }


    }


    if ( ELFCLASS64 == elf_get_machine_class( machine ) ) {
        if ( -1 == elf64_write_sections( fp,
                                         elf_get_machine_endianness( machine ),
                                         text, textsz,
                                         data, datasz,
                                         bss, bsssz,
                                         shstrtab, shstrtabsz,
                                         strtab, strtabsz,
                                         symtab, symtabsz,
                                         reltext, reltextsz,
                                         reldata, reldatasz) ) {
            fclose( fp );
            return -1;
        }
    }
    else {
        if ( -1 == elf32_write_sections( fp,
                                         elf_get_machine_endianness( machine ),
                                         text, textsz,
                                         data, datasz,
                                         bss, bsssz,
                                         shstrtab, shstrtabsz,
                                         strtab, strtabsz,
                                         symtab, symtabsz,
                                         reltext, reltextsz,
                                         reldata, reldatasz) ) {
            fclose( fp );
            return -1;
        }
    }

    fclose( fp );

    return 0;
}

