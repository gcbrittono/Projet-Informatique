
/**
 * @file read.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Sep 22 20:11:13 2017
 * @brief Read ELF.
 *
 * Read ELF.
 */

#include <stdlib.h>
#include <string.h>
#include <libgen.h> /* basename(3) */

#include <pelf/pelf.h>

int   elf_read_ehdr( FILE *fp, Elf_Ehdr *ehdr ) {

    fseek( fp, 0L, SEEK_SET );

    if ( sizeof(*ehdr) != fread( ehdr, 1, sizeof(*ehdr), fp ) ) {
        return -1;
    }

    fseek( fp, 0L, SEEK_SET );

    if ( PELF_HOST_ENDIANNESS != elf_endianness( *ehdr ) ) {
        PELF_DECLARE2( Ehdr, ehdr, ehdr );

        PELF_FLIP2( elf_width( ehdr ), Ehdr, ehdr );
    }

    return elf_width( ehdr );
}

unsigned char *elf_read_symtab( FILE *fp, Elf_Ehdr ehdr, unsigned char *shtab, size_t *symtabsz ) {
    unsigned char       *symtab = NULL;
    size_t                    i = 0;
    int                   width = elf_width( ehdr );
    size_t                shnum = PELF( width, Ehdr, &ehdr, e_shnum );
    PELF_DECLARE2( Shdr, shdr, shtab );

    for ( i = 0; i< shnum; PELF_NEXT2( shdr ), i++ ) {

        *symtabsz = PELF2( width, Shdr, shdr, sh_size );

        if ( SHT_SYMTAB == PELF2( width, Shdr, shdr, sh_type ) ) {
            symtab = calloc( 1, *symtabsz );

            PELF_DECLARE2( Sym, sym, symtab );

            fseek( fp, PELF2( width, Shdr, shdr, sh_offset ), SEEK_SET );
            if ( *symtabsz != fread( symtab, 1, *symtabsz, fp ) ) {
                PELF_ERROR( "Could not read symbol table.\n" );
                free( symtab );
                return NULL;
            }

            if ( PELF_HOST_ENDIANNESS != elf_endianness( ehdr ) ) {
                size_t j;
                if ( ELFCLASS64 == width ) {
                    for ( j = 0; j< (*symtabsz)/PELF_SIZEOF( width, Sym ); j++ ) {
                        elf64_endian_flip_Sym( j+sym64 );
                    }
                }
                else {
                    for ( j = 0; j< (*symtabsz)/PELF_SIZEOF( width, Sym ); j++ ) {
                        elf32_endian_flip_Sym( j+sym32 );
                    }
                }
            }

            return symtab;
        }
    }

    return symtab;
}

unsigned char *elf_read_section_by_name( FILE *fp, Elf_Ehdr ehdr, unsigned char *shstrtab, unsigned char *shtab, size_t *scnsz, char *scname ) {
    unsigned char             *scdata = NULL;
    int                         width = elf_width( ehdr );
    int                    endianness = elf_endianness( ehdr );
    size_t                      shnum = PELF( width, Ehdr, &ehdr, e_shnum );
    size_t                          i = 0;
    PELF_DECLARE2( Shdr, shdr, shtab );

    *scnsz             = 0;

    for ( i = 0; i< shnum; PELF_NEXT2( shdr ), i++ ) {

        if ( ( ELFCLASS64 == width && !strcmp( scname, (char*)shstrtab+shdr64->sh_name ) ) ||
                ( ELFCLASS64 != width && !strcmp( scname, (char*)shstrtab+shdr32->sh_name ) )   ) {


            size_t offset = PELF2( width, Shdr, shdr, sh_offset );

            *scnsz        = PELF2( width, Shdr, shdr, sh_size );
            scdata        = calloc( 1, *scnsz );

            fseek( fp, offset, SEEK_SET );
            fread( scdata, 1, *scnsz, fp );

            if ( *scnsz && scdata && PELF_HOST_ENDIANNESS != endianness ) {
                PELF_DECLARE2( Rel, rel, scdata );
                PELF_DECLARE2( Rela, rela, scdata );
                PELF_DECLARE2( Sym, sym, scdata );
                size_t         i;

                switch( PELF2( width, Shdr, shdr, sh_type ) ) {
                case SHT_REL :
                    for ( i = 0; i< *scnsz/PELF_SIZEOF( width, Rel ); PELF_NEXT2( rel ), i++ ) {
                        PELF_FLIP2( width, Rel, rel );
                    }
                    break;
                case SHT_RELA :
                    for ( i = 0; i< *scnsz/PELF_SIZEOF( width, Rela ); PELF_NEXT2( rela ), i++ ) {
                        PELF_FLIP2( width, Rela, rela );
                    }
                    break;
                case SHT_DYNSYM :
                case SHT_SYMTAB :
                    for ( i = 0; i< *scnsz/PELF_SIZEOF( width, Sym ); PELF_NEXT2( sym ), i++ ) {
                        PELF_FLIP2( width, Sym, sym );
                    }
                    break;
                default :
                    break;
                }
            }

            return scdata;
        }
    }

    return scdata;
}

static int   elf_dump_section_by_name( FILE *fp, Elf_Ehdr ehdr, char *output, char *secname, unsigned char *shstrtab, unsigned char *shtab ) {

    char      *scn = NULL;
    size_t   scnsz = 0;
    FILE    *fpout = NULL;

    fpout = fopen( output, "wb" );
    if ( !fpout ) {
        fprintf( stderr, "Unable to output %s for writing.\n", output );
        return -1;
    }

    scn = (char*) elf_read_section_by_name( fp, ehdr, shstrtab, shtab, &scnsz, secname );

    if ( scn && scnsz ) {
        fwrite( scn, 1, scnsz, fpout );
    }

    fclose( fpout );

    return 0;
}

/* If secname is "all", dump all sections */
int   elf_dump_section( FILE *fp, char *input, char *secname ) {

    char                 output[256];
    unsigned char        *shtab = NULL;
    size_t                shnum = 0;
    unsigned char     *shstrtab = NULL;
    size_t           shstrtabsz = 0;
    size_t                    i = 0;
    char                  *name = 0;
    Elf_Ehdr               ehdr;

    elf_read_ehdr( fp, &ehdr );

    shtab    = elf_read_shtab( fp, ehdr, &shnum );

    shstrtab = elf_read_shstrtab( fp, ehdr, shtab, &shstrtabsz );

    for ( i = 0; i< shnum; i++ ) {
        if ( ELFCLASS64 == elf_width( ehdr ) ) {
            name = (char*)&shstrtab[((Elf64_Shdr*)shtab)[i].sh_name];
        }
        else {
            name = (char*)&shstrtab[((Elf32_Shdr*)shtab)[i].sh_name];
        }

        if ( name ) {
            sprintf( output, "%s%s", basename( input ), name );

            if ( !strcasecmp( "all", secname ) ) {
                elf_dump_section_by_name( fp, ehdr, output, name, shstrtab, shtab );
            }
            else {
                if ( !strcasecmp( secname, name ) ) {
                    elf_dump_section_by_name( fp, ehdr, output, name, shstrtab, shtab );
                    break;
                }
            }
        }
    }

    return 0;
}

unsigned char *elf_read_shtab( FILE *fp, Elf_Ehdr ehdr, size_t *shnum ) {
    unsigned char        *shtab = NULL;
    int                   width = elf_width( ehdr );
    int              endianness = elf_endianness( ehdr );
    size_t               shdrsz = PELF_SIZEOF( width, Shdr );

    *shnum = PELF(width, Ehdr, &ehdr, e_shnum);

    if ( 0 == *shnum ) return NULL;

    shtab  = calloc( *shnum, shdrsz );

    fseek( fp, PELF(width, Ehdr, &ehdr, e_shoff), SEEK_SET );
    if ( shdrsz != fread( shtab, *shnum, shdrsz, fp ) ) {
        free( shtab );
        /* PELF_ERROR( "Could not read enough section header descriptors (wanted %lu)." *shnum ); */
        fprintf( stderr, "Could not read enough section header descriptors (wanted %lu).", *shnum );
        return NULL;
    }

    if ( PELF_HOST_ENDIANNESS != endianness ) {
        unsigned int i;
        PELF_DECLARE2( Shdr, shdr, shtab );

        for ( i = 0; i< *shnum; PELF_NEXT2( shdr ), i++ ) {
            PELF_FLIP2( width, Shdr, shdr );
        }
    }

    return shtab;
}


unsigned char *elf_read_shstrtab( FILE *fp, Elf_Ehdr ehdr, unsigned char *shtab, size_t *shstrtabsz ) {
    unsigned char     *shstrtab = NULL;
    int                   width = elf_width( ehdr );
    size_t                index = PELF( width, Ehdr, &ehdr, e_shstrndx );
    size_t               offset = 0;
    size_t                    i;
    PELF_DECLARE2( Shdr, shdr, shtab );

    for ( i = 0; i< index; PELF_NEXT2( shdr ), i++ );

    *shstrtabsz = PELF2( width, Shdr, shdr, sh_size );

    fseek( fp, PELF2( width, Shdr, shdr, sh_offset ), SEEK_SET );

    shstrtab = calloc( 1, *shstrtabsz );
    if ( *shstrtabsz != fread( shstrtab, 1, *shstrtabsz, fp ) ) {
        PELF_ERROR( "Unable to read .shstrtab.\n" );
        free( shstrtab );
        return NULL;
    }

    return shstrtab;
}

unsigned char *elf_read_strtab( FILE *fp, Elf_Ehdr ehdr, unsigned char *shtab, size_t *strtabsz ) {
    unsigned char       *strtab = NULL;
    int                   width = elf_width( ehdr );
    size_t                    i = 0;
    size_t                shnum = PELF( width, Ehdr, &ehdr, e_shnum );
    PELF_DECLARE2( Shdr, shdr, shtab );

    for ( i = 0; i< shnum; PELF_NEXT2( shdr ), i++ ) {

        if ( PELF( width, Ehdr, &ehdr, e_shstrndx ) != i &&
                SHT_STRTAB == PELF2( width, Shdr, shdr, sh_type ) ) {

            size_t offset = PELF2( width, Shdr, shdr, sh_offset );
            *strtabsz     = PELF2( width, Shdr, shdr, sh_size );

            strtab = calloc( 1, *strtabsz );
            fseek( fp, offset, SEEK_SET );
            if ( *strtabsz != fread( strtab, 1, *strtabsz, fp ) ) {
                PELF_ERROR( "Unable to read .strtab.\n" );
                free( strtab );
                return NULL;
            }

            return strtab;
        }
    }

    return strtab;
}

